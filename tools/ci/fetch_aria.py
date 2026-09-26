#!/usr/bin/env python3
"""Fetch Aria at a hard-pinned commit into <repo>/build/deps/aria.

Replaces the former third_party/aria git submodule: no submodule init and
no vendored tree -- the framework is cloned once at a pinned SHA, verified,
and reused across builds. CMake configure stays offline; run this script
once (and after changing ARIA_SHA) before configuring.
"""
import shutil
import subprocess
import sys
from pathlib import Path

ARIA_URL = "https://github.com/dqsjqian/Aria.git"
ARIA_SHA = "d8e763e00219791ecd75fc1d97fcdc48100651bf"
DEST = Path(__file__).resolve().parents[2] / "build" / "deps" / "aria"


def git(*args):
    result = subprocess.run(["git", *args], capture_output=True)
    if result.returncode != 0:
        raise RuntimeError("git %s failed: %s"
                           % (" ".join(args),
                              result.stderr.decode("utf-8", "replace")))
    return result.stdout.decode("utf-8", "replace")


def main():
    DEST.parent.mkdir(parents=True, exist_ok=True)
    pin_file = DEST / ".pinned-aria-sha"
    if (DEST / ".git").exists():
        pinned = pin_file.read_text(encoding="utf-8").strip() \
            if pin_file.is_file() else ""
        if pinned == ARIA_SHA:
            print("Aria already pinned at %s -> %s" % (ARIA_SHA[:10], DEST))
            return 0
        # Different pinned version: wipe and re-fetch; stale build trees are
        # reconfigured by CMake automatically.
        shutil.rmtree(DEST)
    git("init", str(DEST))
    git("-C", str(DEST), "remote", "add", "origin", ARIA_URL)
    try:
        git("-C", str(DEST), "fetch", "--depth", "1", "origin", ARIA_SHA)
    except RuntimeError:
        # Fallback for hosts that refuse fetch-by-SHA: pull main fully and
        # check out the pinned commit from it.
        git("-C", str(DEST), "fetch", "origin", "main")
    git("-C", str(DEST), "checkout", "--force", ARIA_SHA)
    pin_file.write_text(ARIA_SHA, encoding="utf-8")
    print("Aria pinned at %s -> %s" % (ARIA_SHA[:10], DEST))
    return 0


if __name__ == "__main__":
    sys.exit(main())
