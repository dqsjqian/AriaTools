#!/usr/bin/env bash
# ============================================================================
#  gen-ios.sh — 生成 iOS (UIKit) Xcode 工程并（可选）构建。
#
#  复用 core/（纯 C++）+ aria::adapters::uikit，View 层为 platform/ios（UIKit .mm）。
#
#  用法：
#    bash Workbench/scripts/gen-ios.sh              # 生成 Xcode 工程
#    bash Workbench/scripts/gen-ios.sh build         # 生成 + 编译（iOS 模拟器）
#    bash Workbench/scripts/gen-ios.sh open          # 生成 + 打开 Xcode
#    bash Workbench/scripts/gen-ios.sh clean
#
#  需要：Xcode + iOS SDK（xcodebuild）。
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/ios"

MODE="${1:-generate}"

if [[ "$MODE" == "clean" ]]; then
  echo "[gen-ios] 清理 $BUILD_DIR"
  rm -rf "$BUILD_DIR"
  exit 0
fi

# Xcode 生成器需要「完整 Xcode」，而非 Command Line Tools。
# 若当前 xcode-select 指向 CLT，则尝试用 DEVELOPER_DIR 临时指向已安装的 Xcode
# （不改系统设置、不需 sudo，仅对本次构建生效）。
if ! xcodebuild -version >/dev/null 2>&1; then
  for xc in /Applications/Xcode.app /Applications/Xcode-beta.app; do
    if [[ -d "$xc/Contents/Developer" ]]; then
      export DEVELOPER_DIR="$xc/Contents/Developer"
      echo "[gen-ios] 临时使用 Xcode: $DEVELOPER_DIR"
      break
    fi
  done
fi

if ! xcodebuild -version >/dev/null 2>&1; then
  echo "[gen-ios] 错误：需要完整 Xcode（当前仅有 Command Line Tools）。" >&2
  echo "[gen-ios] 请安装 Xcode，或运行: sudo xcode-select -s /Applications/Xcode.app/Contents/Developer" >&2
  exit 1
fi

# 用 Xcode 生成器 + iOS 系统名生成工程（面向模拟器，开发期免签名）。
cmake -S "$WB_ROOT" -B "$BUILD_DIR" -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphonesimulator \
  -DCMAKE_OSX_ARCHITECTURES="arm64" \
  -DWORKBENCH_TARGET_IOS=ON \
  -DWORKBENCH_TARGET_QT=OFF

echo ""
echo "[gen-ios] ✅ Xcode 工程已生成: $BUILD_DIR/workbench.xcodeproj"

case "$MODE" in
  build)
    echo "[gen-ios] 编译（iOS 模拟器）..."
    cmake --build "$BUILD_DIR" --config Debug -- -sdk iphonesimulator
    echo "[gen-ios] ✅ 编译完成"
    ;;
  open)
    open "$BUILD_DIR/workbench.xcodeproj"
    ;;
esac
