#!/usr/bin/env python3
"""Fetch the pinned Aria revision without overwriting local dependency edits.

Use --source /path/to/Aria or ARIA_SOURCE to consume a local commit before it
is published.
The commit ID is verified on every run; a marker file is only informational.
A successful version change retains the old checkout under build/deps/aria-backup-*.
"""
import argparse
import os
import subprocess
import sys
import tempfile
from pathlib import Path

ARIA_URL = "https://github.com/dqsjqian/Aria.git"
ARIA_SHA = "202f0b8e56a5572bf6f6e1f018ccbdd112690ed6"
DEST = Path(__file__).resolve().parents[2] / "build" / "deps" / "aria"


def git(*args):
    result = subprocess.run(["git", *map(str, args)], capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError("git %s failed: %s" % (" ".join(map(str, args)), result.stderr.strip()))
    return result.stdout.strip()


def write_pin(checkout):
    # Never follow a marker symlink supplied by an existing checkout or by
    # the fetched tree. The marker is informational, so replace its directory
    # entry atomically without touching whatever a symlink used to reference.
    marker = checkout / ".pinned-aria-sha"
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", dir=checkout,
                                         prefix=".aria-pin-", delete=False) as handle:
            temporary = Path(handle.name)
            handle.write(ARIA_SHA + "\n")
        temporary.replace(marker)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", default=os.environ.get("ARIA_SOURCE", ARIA_URL),
                        help="Git URL or local Aria repository (defaults to ARIA_SOURCE or upstream; same pinned SHA)")
    args = parser.parse_args(argv)
    source = str(Path(args.source).resolve()) if Path(args.source).exists() else args.source
    DEST.parent.mkdir(parents=True, exist_ok=True)
    lock = DEST.parent / ".aria-fetch.lock"
    try:
        lock.mkdir()
    except FileExistsError as error:
        raise RuntimeError("Another Aria fetch owns %s; if it crashed, remove this stale lock" % lock) from error
    try:
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
            if git("-C", DEST, "rev-parse", "HEAD") == ARIA_SHA:
                write_pin(DEST)
                print("Aria verified at %s -> %s" % (ARIA_SHA[:12], DEST))
                return 0
        # Prepare and verify completely before changing the current checkout.
        with tempfile.TemporaryDirectory(prefix=".aria-fetch-", dir=DEST.parent) as temporary:
            staged = Path(temporary) / "aria"
            git("init", staged)
            git("-C", staged, "remote", "add", "origin", source)
            try:
                git("-C", staged, "fetch", "--depth", "1", "origin", ARIA_SHA)
            except RuntimeError:
                git("-C", staged, "fetch", "origin", "main")
            git("-C", staged, "checkout", "--detach", ARIA_SHA)
            actual = git("-C", staged, "rev-parse", "HEAD")
            if actual != ARIA_SHA:
                raise RuntimeError("Aria revision mismatch: expected %s, got %s" % (ARIA_SHA, actual))
            write_pin(staged)
            backup = None
            if DEST.exists():
                backup = Path(tempfile.mkdtemp(prefix="aria-backup-", dir=DEST.parent))
                backup.rmdir()
                DEST.rename(backup)
            try:
                staged.rename(DEST)
            except OSError:
                if backup is not None:
                    backup.rename(DEST)
                raise
            if backup is not None:
                print("Previous Aria checkout retained at %s" % backup)
        print("Aria pinned at %s -> %s" % (ARIA_SHA[:12], DEST))
        return 0
    finally:
        lock.rmdir()


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError) as error:
        print("error: %s" % error, file=sys.stderr)
        sys.exit(1)
