#!/usr/bin/env bash
# ============================================================================
#  gen-ios.sh — Generate the iOS (UIKit) Xcode project and (optionally) build.
#
#  Reuses core/ (pure C++) + aria::adapters::uikit; the View layer is platform/ios (UIKit .mm).
#
#  Usage:
#    bash Workbench/scripts/gen-ios.sh              # generate Xcode project
#    bash Workbench/scripts/gen-ios.sh build         # generate + build (iOS simulator)
#    bash Workbench/scripts/gen-ios.sh open          # generate + open Xcode
#    bash Workbench/scripts/gen-ios.sh clean
#
#  Requires: Xcode + iOS SDK (xcodebuild).
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/ios"

MODE="${1:-generate}"

# Device build Team (personal Apple ID: dqsjqian@163.com). Override via environment variable.
WB_IOS_DEV_TEAM="${WB_IOS_DEV_TEAM:-C9SMVLK586}"

if [[ "$MODE" == "clean" ]]; then
  echo "[gen-ios] Cleaning $BUILD_DIR and ${BUILD_DIR}-device"
  rm -rf "$BUILD_DIR" "${BUILD_DIR}-device"
  exit 0
fi

# Device mode uses a separate build directory to avoid mixing with the simulator SDK.
if [[ "$MODE" == "device" ]]; then
  BUILD_DIR="${BUILD_DIR}-device"
fi

# The Xcode generator requires a full Xcode install, not just Command Line Tools.
# If xcode-select currently points to CLT, try to point DEVELOPER_DIR at an installed Xcode
# (does not change system settings, no sudo needed, only effective for this build).
if ! xcodebuild -version >/dev/null 2>&1; then
  for xc in /Applications/Xcode.app /Applications/Xcode-beta.app; do
    if [[ -d "$xc/Contents/Developer" ]]; then
      export DEVELOPER_DIR="$xc/Contents/Developer"
      echo "[gen-ios] Temporarily using Xcode: $DEVELOPER_DIR"
      break
    fi
  done
fi

if ! xcodebuild -version >/dev/null 2>&1; then
  echo "[gen-ios] Error: full Xcode is required (only Command Line Tools found)." >&2
  echo "[gen-ios] Install Xcode, or run: sudo xcode-select -s /Applications/Xcode.app/Contents/Developer" >&2
  exit 1
fi

if [[ "$MODE" == "device" ]]; then
  # Device: iphoneos SDK + automatic signing (passing Team → CMake enables signing).
  echo "[gen-ios] Generating device project (iphoneos, Team=${WB_IOS_DEV_TEAM}) ..."
  cmake -S "$WB_ROOT" -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphoneos \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DWORKBENCH_TARGET_IOS=ON \
    -DWORKBENCH_TARGET_QT=OFF \
    -DWB_IOS_DEV_TEAM="$WB_IOS_DEV_TEAM"
else
  # Simulator: iphonesimulator SDK, no signing required.
  cmake -S "$WB_ROOT" -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DWORKBENCH_TARGET_IOS=ON \
    -DWORKBENCH_TARGET_QT=OFF
fi

echo ""
echo "[gen-ios] ✅ Xcode project generated: $BUILD_DIR/workbench.xcodeproj"

case "$MODE" in
  build)
    echo "[gen-ios] Building (iOS simulator)..."
    cmake --build "$BUILD_DIR" --config Debug -- -sdk iphonesimulator
    echo "[gen-ios] ✅ Build complete"
    ;;
  device)
    echo "[gen-ios] Building (device, automatic signing)..."
    # -allowProvisioningUpdates: auto-generate a profile when none is local (personal Team must be signed in to Xcode).
    xcodebuild -project "$BUILD_DIR/workbench.xcodeproj" \
      -target workbench -configuration Debug \
      -sdk iphoneos -destination 'generic/platform=iOS' \
      -allowProvisioningUpdates \
      DEVELOPMENT_TEAM="$WB_IOS_DEV_TEAM"
    APP="$(/usr/bin/find "$BUILD_DIR" -maxdepth 4 -name 'workbench.app' -path '*Debug-iphoneos*' | head -1)"
    if [[ -z "$APP" ]]; then echo "[gen-ios] Error: device .app artifact not found" >&2; exit 1; fi
    echo "[gen-ios] ✅ Build complete: $APP"

    # Pick a connected device (devicectl). Use WB_IOS_DEVICE to specify an identifier.
    # Device names may contain spaces, so extract the UUID (8-4-4-4-12) via regex rather than by column.
    DEV="${WB_IOS_DEVICE:-$(xcrun devicectl list devices 2>/dev/null \
        | grep 'connected' | grep 'physical' \
        | grep -oE '[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}' \
        | head -1)}"
    if [[ -z "$DEV" ]]; then
      echo "[gen-ios] No connected device detected, skipping install. Artifact at: $APP"
      echo "[gen-ios] After connecting a device, run:"
      echo "  xcrun devicectl device install app --device <ID> \"$APP\""
      exit 0
    fi
    echo "[gen-ios] Installing to device $DEV ..."
    xcrun devicectl device install app --device "$DEV" "$APP"
    echo "[gen-ios] Launching app ..."
    xcrun devicectl device process launch --device "$DEV" com.dqsjqian.workbench || \
      echo "[gen-ios] Launch failed: for first-time installs, trust the developer under Settings → General → VPN & Device Management on the device and retry."
    echo "[gen-ios] ✅ Done"
    ;;
  open)
    open "$BUILD_DIR/workbench.xcodeproj"
    ;;
esac
