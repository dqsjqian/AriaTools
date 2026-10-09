#!/usr/bin/env python3
"""Resolve and fetch a stable Aria release without overwriting local edits.

Explicit --version / ARIA_DEP_ARIA_VERSION wins; otherwise reuse the recorded selection or
resolve latest stable on first use. --update refreshes the selection.
--source / ARIA_SOURCE changes the Git source, never the required commit.
The commit ID is verified on every run; a marker file is only informational.
A successful version change retains the old checkout under build/deps/aria-backup-*.
"""
import argparse
import os
import subprocess
import sys
import tempfile
from pathlib import Path

from dependencies import Context, atomic_json, load_json, lock_output, resolve

ROOT = Path(__file__).resolve().parents[2]
DEST = ROOT / "build" / "deps" / "aria"


def git(*args):
    result = subprocess.run(["git", *map(str, args)], capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError("git %s failed: %s" % (" ".join(map(str, args)), result.stderr.strip()))
    return result.stdout.strip()


def write_pin(checkout, revision):
    # Never follow a marker symlink supplied by an existing checkout or by
    # the fetched tree. The marker is informational, so replace its directory
    # entry atomically without touching whatever a symlink used to reference.
    marker = checkout / ".pinned-aria-sha"
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", dir=checkout,
                                         prefix=".aria-pin-", delete=False) as handle:
            temporary = Path(handle.name)
            handle.write(revision + "\n")
        temporary.replace(marker)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", default=os.environ.get("ARIA_SOURCE"),
                        help="Override the locked Git URL with a local repository or mirror")
    parser.add_argument("--version", default=os.environ.get("ARIA_DEP_ARIA_VERSION"),
                        help="Explicit stable release version for this invocation")
    parser.add_argument("--file", type=Path, default=ROOT / "dependencies.json",
                        help="Dependency requirements and embedded resolved results")
    parser.add_argument("--update", action="store_true", help="Resolve the selection again")
    parser.add_argument("--offline", action="store_true", help="Require a recorded selection and local checkout/source")
    args = parser.parse_args(argv)
    DEST.parent.mkdir(parents=True, exist_ok=True)
    mutex = DEST.parent / ".aria-fetch.lock"
    try:
        mutex.mkdir()
    except FileExistsError as error:
        raise RuntimeError("Another Aria fetch owns %s; if it crashed, remove this stale lock" % mutex) from error
    try:
        # Hold the real lock while staging both metadata and checkout. A failed
        # download/install must never publish a lock that describes another tree.
        with lock_output(args.file), tempfile.TemporaryDirectory(prefix=".aria-fetch-", dir=DEST.parent) as temporary:
            transaction = Path(temporary)
            effective = transaction / "dependencies.json"
            selection = resolve(args.file, effective, only=["aria"], update=args.update,
                                versions={"aria": args.version} if args.version else {},
                                context=Context(DEST.parent / "resolver", offline=args.offline))
            record = selection["dependencies"]["aria"]
            if record["source"].get("artifact") != "git":
                raise ValueError("Aria bootstrap requires a Git dependency declaration")
            revision = record["revision"]
            source = args.source or record["url"]
            source = str(Path(source).resolve()) if Path(source).exists() else source
            payload = load_json(effective)
            payload.pop("_base_sha256", None)
            return install(source, revision, transaction, args.file, payload, args.offline)
    finally:
        mutex.rmdir()


def install(source, revision, transaction, lock_path, selection, offline=False):
    if DEST.is_symlink():
        raise RuntimeError("Refusing to replace a symlink dependency directory: %s" % DEST)
    if DEST.exists():
        if not (DEST / ".git").exists():
            raise RuntimeError("Refusing to replace a non-Git directory: %s" % DEST)
        if (DEST / ".git").is_file():
            # Renaming a linked worktree/submodule does not update its
            # external Git administration paths. Its backup would become
            # unusable (or refer to the replacement checkout).
            raise RuntimeError("Refusing to relocate a linked worktree or submodule: %s" % DEST)
        dirty = git("-C", DEST, "status", "--porcelain", "--untracked-files=all",
                    "--", ".", ":(exclude).pinned-aria-sha")
        if dirty:
            raise RuntimeError("Aria dependency has local edits; preserve them before updating:\n%s" % dirty)
        if git("-C", DEST, "rev-parse", "HEAD") == revision:
            write_pin(DEST, revision)
            atomic_json(lock_path, selection)
            print("Aria verified at %s -> %s" % (revision[:12], DEST))
            return 0
    # Prepare and verify completely before changing the current checkout.
    if offline and not Path(source).exists():
        raise RuntimeError("Offline fetch requires the locked checkout or a local Git source")
    staged = transaction / "aria"
    git("init", staged)
    git("-C", staged, "remote", "add", "origin", source)
    try:
        git("-C", staged, "fetch", "--depth", "1", "origin", revision)
    except RuntimeError:
        git("-C", staged, "fetch", "origin", "main")
    git("-C", staged, "checkout", "--detach", revision)
    actual = git("-C", staged, "rev-parse", "HEAD")
    if actual != revision:
        raise RuntimeError("Aria revision mismatch: expected %s, got %s" % (revision, actual))
    write_pin(staged, revision)
    backup = None
    if DEST.exists():
        backup = Path(tempfile.mkdtemp(prefix="aria-backup-", dir=DEST.parent))
        backup.rmdir()
        DEST.rename(backup)
    try:
        staged.rename(DEST)
        atomic_json(lock_path, selection)
    except (OSError, ValueError):
        if DEST.exists():
            DEST.rename(staged)
        if backup is not None:
            backup.rename(DEST)
        raise
    if backup is not None:
        print("Previous Aria checkout retained at %s" % backup)
    print("Aria pinned at %s -> %s" % (revision[:12], DEST))
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print("error: %s" % error, file=sys.stderr)
        sys.exit(1)
