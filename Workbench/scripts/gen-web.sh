#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/web"
MODE="${1:-build}"

if [[ "$MODE" == "clean" ]]; then
  rm -rf "$BUILD_DIR"
  exit 0
fi

case "$MODE" in
  build|run|probe) ;;
  *)
    echo "unknown mode: $MODE"
    echo "valid: build | run | probe | clean"
    exit 1
    ;;
esac

cmake -S "$WB_ROOT" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWORKBENCH_TARGET_WEB=ON \
  -DWORKBENCH_TARGET_QT=OFF
cmake --build "$BUILD_DIR" -j "$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)"

BIN="$BUILD_DIR/platform/web/workbench_web"
if [[ "$MODE" == "probe" ]]; then
  "$BIN" --probe
  exit 0
fi

if [[ "$MODE" == "run" ]]; then
  exec "$BIN"
fi

echo "[gen-web] Build complete: $BIN"
echo "[gen-web] Run: bash Workbench/scripts/gen-web.sh run"
echo "[gen-web] Probe: bash Workbench/scripts/gen-web.sh probe"
