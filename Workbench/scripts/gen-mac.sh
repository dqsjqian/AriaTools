#!/usr/bin/env bash
# ============================================================================
#  gen-mac.sh — One-shot generate and build Mac (Qt6) project
#
#  Usage:
#    bash Workbench/scripts/gen-mac.sh           # configure + build (Release)
#    bash Workbench/scripts/gen-mac.sh debug      # Debug
#    bash Workbench/scripts/gen-mac.sh clean      # clean build/mac
#    bash Workbench/scripts/gen-mac.sh run        # run after build
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"      # Workbench/
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"       # repository root
BUILD_DIR="$REPO_ROOT/build/mac"

MODE="${1:-release}"

case "$MODE" in
  clean)
    echo "[gen-mac] Cleaning $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    exit 0
    ;;
esac

# ── Locate Homebrew Qt6 ─────────────────────────────────────────────────────
QT_PREFIX="$(brew --prefix qt 2>/dev/null || true)"
if [[ -z "$QT_PREFIX" || ! -d "$QT_PREFIX" ]]; then
  echo "[gen-mac] Error: Homebrew Qt6 not found. Run: brew install qt" >&2
  exit 1
fi
echo "[gen-mac] Qt6 prefix: $QT_PREFIX"

BUILD_TYPE="Release"
[[ "$MODE" == "debug" ]] && BUILD_TYPE="Debug"

# ── Configure ───────────────────────────────────────────────────────────────
GEN_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GEN_ARGS+=(-G Ninja)
fi

cmake -S "$WB_ROOT" -B "$BUILD_DIR" \
  ${GEN_ARGS[@]+"${GEN_ARGS[@]}"} \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DWORKBENCH_TARGET_QT=ON \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX"

# ── Build ───────────────────────────────────────────────────────────────────
cmake --build "$BUILD_DIR" -j "$(sysctl -n hw.ncpu)"

APP_PATH="$BUILD_DIR/platform/qt/workbench.app"
echo ""
echo "[gen-mac] ✅ Build complete: $APP_PATH"

if [[ "$MODE" == "run" ]]; then
  echo "[gen-mac] Launching app..."
  open "$APP_PATH"
fi
