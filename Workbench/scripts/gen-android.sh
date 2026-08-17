#!/usr/bin/env bash
# ============================================================================
#  gen-android.sh — Cross-build the Workbench Android app.
#
#  Two stages:
#    1. NDK cross-build of the pure-C++ core + modules into
#       build/platforms/android/  (static .a archives + i18n resources).
#    2. Gradle (platform/android/app) assembles the Kotlin/Compose shell and
#       the JNI bridge (aria_jni.so) that links those .a archives.
#
#  Usage:
#    scripts/gen-android.sh            # stage 1 only (core static libs)
#    scripts/gen-android.sh --apk      # stage 1 + stage 2 (assemble APK)
#    scripts/gen-android.sh clean      # wipe build/platforms/android/
#
#  Env:
#    ANDROID_SDK_ROOT    Android SDK path (default ~/Library/Android/sdk)
#    ANDROID_NDK_ROOT    Android NDK path (default latest under SDK)
#    ARIA_ANDROID_CMAKE  CMake path (default SDK-bundled latest)
#    ARIA_ANDROID_NINJA  ninja path (default beside the SDK cmake)
#    JOBS                parallel jobs (default CPU count)
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WB_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"      # Workbench/
REPO_ROOT="$(cd "$WB_ROOT/.." && pwd)"       # repository root (AriaTools/)
cd "$REPO_ROOT"

MODE="${1:-core}"
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

# ── Android toolchain detection ─────────────────────────────────────────────
ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$HOME/Library/Android/sdk}"
if [[ ! -d "$ANDROID_SDK_ROOT" ]]; then
    echo "✗ ANDROID_SDK_ROOT not found: $ANDROID_SDK_ROOT" >&2
    echo "  Set ANDROID_SDK_ROOT or install Android SDK" >&2
    exit 1
fi
ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_SDK_ROOT/ndk/$(ls "$ANDROID_SDK_ROOT/ndk/" 2>/dev/null | sort -V | tail -1)}"
ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT%/}"
if [[ ! -d "$ANDROID_NDK_ROOT" ]]; then
    echo "✗ Android NDK not found at: $ANDROID_NDK_ROOT" >&2
    exit 1
fi
ARIA_ANDROID_CMAKE="${ARIA_ANDROID_CMAKE:-$ANDROID_SDK_ROOT/cmake/$(ls "$ANDROID_SDK_ROOT/cmake/" 2>/dev/null | sort -V | tail -1)/bin/cmake}"
if [[ ! -x "$ARIA_ANDROID_CMAKE" ]]; then
    ARIA_ANDROID_CMAKE="$(command -v cmake 2>/dev/null || true)"
fi
if [[ -z "$ARIA_ANDROID_CMAKE" ]]; then
    echo "✗ CMake not found. Install CMake or set ARIA_ANDROID_CMAKE" >&2
    exit 1
fi
ARIA_ANDROID_NINJA="${ARIA_ANDROID_NINJA:-$(dirname "$ARIA_ANDROID_CMAKE")/ninja}"
if [[ ! -x "$ARIA_ANDROID_NINJA" ]]; then
    ARIA_ANDROID_NINJA="$(command -v ninja 2>/dev/null || true)"
fi
if [[ -z "$ARIA_ANDROID_NINJA" ]]; then
    echo "✗ ninja not found. Install ninja or set ARIA_ANDROID_NINJA" >&2
    exit 1
fi

BUILD_DIR="build/platforms/android"

case "$MODE" in
    clean)
        rm -rf "$BUILD_DIR" platform/android/app/build platform/android/app/.cxx
        echo "✓ android build trees wiped"
        exit 0
        ;;
    core|--apk) ;;
    *) echo "usage: gen-android.sh [core|--apk|clean]"; exit 1 ;;
esac

echo "▶ Workbench Android core cross-build (NDK)"
echo "  NDK   : $ANDROID_NDK_ROOT"
echo "  CMake : $ARIA_ANDROID_CMAKE"
echo "  Ninja : $ARIA_ANDROID_NINJA"
echo "  Build : $BUILD_DIR"

# ── Stage 1: NDK cross-build of the pure-C++ core + modules ────────────────
# WORKBENCH_TARGET_ANDROID=ON builds wb_utils/wb_infra/wb_module_api/
# wb_core_app/wb_module_* + the Aria JNI adapter as STATIC archives, with no
# view shell (the Kotlin/Compose layer lives in platform/android/app).
mkdir -p "$BUILD_DIR"
"$ARIA_ANDROID_CMAKE" -S "$WB_ROOT" -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_MAKE_PROGRAM="$ARIA_ANDROID_NINJA" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DWORKBENCH_TARGET_ANDROID=ON \
    -DWORKBENCH_TARGET_QT=OFF
"$ARIA_ANDROID_CMAKE" --build "$BUILD_DIR" -j "$JOBS"
# aria_adapters_jni is EXCLUDE_FROM_ALL (no consumer in the core build) but
# the Gradle JNI bridge links it — build it explicitly.
"$ARIA_ANDROID_CMAKE" --build "$BUILD_DIR" -j "$JOBS" --target aria_adapters_jni

# ── Collect all static archives into one lib/ dir ──────────────────────────
# wb_* libs land in per-subdir build trees (core/app/libwb_core_app.a, ...)
# while aria_* are installed into $BUILD_DIR/lib/. The Gradle JNI bridge links
# them from a single flat dir, mirroring demo5's ${ARIA_PREFIX}/lib/*.a layout.
echo "▶ collecting static archives → $BUILD_DIR/lib/"
mkdir -p "$BUILD_DIR/lib"
find "$BUILD_DIR" -name "*.a" -not -path "*CMakeFiles*" \
    -exec cp -f {} "$BUILD_DIR/lib/" \;

echo "✓ core static libs: $BUILD_DIR/lib/"
echo "  i18n resources : $BUILD_DIR/i18n/"

# ── Stage 2: Gradle assemble (optional) ─────────────────────────────────────
if [[ "$MODE" == "--apk" ]]; then
    echo "▶ Gradle assembleDebug (platform/android)"
    APP_DIR="$REPO_ROOT/Workbench/platform/android"
    GRADLE="${GRADLE:-$APP_DIR/gradlew}"
    if [[ ! -x "$GRADLE" ]]; then
        GRADLE="$(command -v gradle || true)"
    fi
    if [[ -z "$GRADLE" ]]; then
        echo "✗ gradle not found; run --apk with gradle on PATH or set GRADLE" >&2
        exit 1
    fi
    (cd "$APP_DIR" && "$GRADLE" assembleDebug -PwbNativeRoot="$REPO_ROOT")
    echo "✓ APK: $APP_DIR/app/build/outputs/apk/debug/app-debug.apk"
fi
