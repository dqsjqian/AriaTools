#!/usr/bin/env python3
"""Bound GUI/web probes to 120 seconds; inherited lease stays active."""
import subprocess
import sys

try:
    raise SystemExit(subprocess.run([sys.argv[1], "--probe"], timeout=120).returncode)
except subprocess.TimeoutExpired:
    print("probe timed out after 120 seconds", file=sys.stderr)
    raise SystemExit(124)
