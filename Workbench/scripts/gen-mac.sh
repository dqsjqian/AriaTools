#!/usr/bin/env bash
# ============================================================================
#  gen-mac.sh — 一键生成并构建 Mac (Qt6) 工程
#
#  用法：
#    bash Workbench/scripts/gen-mac.sh           # 配置 + 构建 (Release)
#    bash Workbench/scripts/gen-mac.sh debug      # Debug
#    bash Workbench/scripts/gen-mac.sh clean      # 清理 build/mac
#    bash Workbench/scripts/gen-mac.sh run        # 构建后直接运行
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"      # Workbench/
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"       # 仓库根
BUILD_DIR="$REPO_ROOT/build/mac"

MODE="${1:-release}"

case "$MODE" in
  clean)
    echo "[gen-mac] 清理 $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    exit 0
    ;;
esac

# ── 定位 Homebrew Qt6 ───────────────────────────────────────────────────────
QT_PREFIX="$(brew --prefix qt 2>/dev/null || true)"
if [[ -z "$QT_PREFIX" || ! -d "$QT_PREFIX" ]]; then
  echo "[gen-mac] 错误：未找到 Homebrew Qt6。请先运行: brew install qt" >&2
  exit 1
fi
echo "[gen-mac] Qt6 前缀: $QT_PREFIX"

BUILD_TYPE="Release"
[[ "$MODE" == "debug" ]] && BUILD_TYPE="Debug"

# ── 配置 ────────────────────────────────────────────────────────────────────
GEN_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GEN_ARGS+=(-G Ninja)
fi

cmake -S "$WB_ROOT" -B "$BUILD_DIR" \
  ${GEN_ARGS[@]+"${GEN_ARGS[@]}"} \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DWORKBENCH_TARGET_QT=ON \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX"

# ── 构建 ────────────────────────────────────────────────────────────────────
cmake --build "$BUILD_DIR" -j "$(sysctl -n hw.ncpu)"

APP_PATH="$BUILD_DIR/platform/qt/workbench.app"
echo ""
echo "[gen-mac] ✅ 构建完成: $APP_PATH"

if [[ "$MODE" == "run" ]]; then
  echo "[gen-mac] 启动应用..."
  open "$APP_PATH"
fi
