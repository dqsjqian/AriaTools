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

# 真机开发 Team（个人 Apple ID: dqsjqian@163.com）。可用环境变量覆盖。
WB_IOS_DEV_TEAM="${WB_IOS_DEV_TEAM:-C9SMVLK586}"

if [[ "$MODE" == "clean" ]]; then
  echo "[gen-ios] 清理 $BUILD_DIR 与 ${BUILD_DIR}-device"
  rm -rf "$BUILD_DIR" "${BUILD_DIR}-device"
  exit 0
fi

# 真机模式用独立 build 目录，避免与模拟器 SDK 混淆。
if [[ "$MODE" == "device" ]]; then
  BUILD_DIR="${BUILD_DIR}-device"
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

if [[ "$MODE" == "device" ]]; then
  # 真机：iphoneos SDK + 自动签名（传入 Team → CMake 开启签名）。
  echo "[gen-ios] 生成真机工程 (iphoneos, Team=${WB_IOS_DEV_TEAM}) ..."
  cmake -S "$WB_ROOT" -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphoneos \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DWORKBENCH_TARGET_IOS=ON \
    -DWORKBENCH_TARGET_QT=OFF \
    -DWB_IOS_DEV_TEAM="$WB_IOS_DEV_TEAM"
else
  # 模拟器：iphonesimulator SDK + 免签名。
  cmake -S "$WB_ROOT" -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DWORKBENCH_TARGET_IOS=ON \
    -DWORKBENCH_TARGET_QT=OFF
fi

echo ""
echo "[gen-ios] ✅ Xcode 工程已生成: $BUILD_DIR/workbench.xcodeproj"

case "$MODE" in
  build)
    echo "[gen-ios] 编译（iOS 模拟器）..."
    cmake --build "$BUILD_DIR" --config Debug -- -sdk iphonesimulator
    echo "[gen-ios] ✅ 编译完成"
    ;;
  device)
    echo "[gen-ios] 编译（真机，自动签名）..."
    # -allowProvisioningUpdates：无本地 profile 时自动生成（个人 Team 需已登录 Xcode 账号）。
    xcodebuild -project "$BUILD_DIR/workbench.xcodeproj" \
      -target workbench -configuration Debug \
      -sdk iphoneos -destination 'generic/platform=iOS' \
      -allowProvisioningUpdates \
      DEVELOPMENT_TEAM="$WB_IOS_DEV_TEAM"
    APP="$(/usr/bin/find "$BUILD_DIR" -maxdepth 4 -name 'workbench.app' -path '*Debug-iphoneos*' | head -1)"
    if [[ -z "$APP" ]]; then echo "[gen-ios] 错误：未找到真机 .app 产物" >&2; exit 1; fi
    echo "[gen-ios] ✅ 编译完成: $APP"

    # 选连接中的真机（devicectl）。可用 WB_IOS_DEVICE 指定 identifier。
    # 设备名可能含空格，故用正则抓 UUID（8-4-4-4-12）而非按列取。
    DEV="${WB_IOS_DEVICE:-$(xcrun devicectl list devices 2>/dev/null \
        | grep 'connected' | grep 'physical' \
        | grep -oE '[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}' \
        | head -1)}"
    if [[ -z "$DEV" ]]; then
      echo "[gen-ios] 未检测到已连接真机，跳过安装。产物在: $APP"
      echo "[gen-ios] 连接设备后可执行："
      echo "  xcrun devicectl device install app --device <ID> \"$APP\""
      exit 0
    fi
    echo "[gen-ios] 安装到设备 $DEV ..."
    xcrun devicectl device install app --device "$DEV" "$APP"
    echo "[gen-ios] 启动 App ..."
    xcrun devicectl device process launch --device "$DEV" com.dqsjqian.workbench || \
      echo "[gen-ios] 启动失败：若首次安装，请在设备上「设置→通用→VPN与设备管理」信任开发者后重试。"
    echo "[gen-ios] ✅ 完成"
    ;;
  open)
    open "$BUILD_DIR/workbench.xcodeproj"
    ;;
esac
