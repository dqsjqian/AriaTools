#!/usr/bin/env bash
# ============================================================================
#  gen-android.sh — Generate the Android (JNI) Gradle project  [plugged in at a later phase]
#
#  Plan:
#    - Reuse the core/ static library (NDK r26+ cross-compiled)
#    - View layer uses native Kotlin/Compose + aria::adapters::jni
#    - CMake: -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_JNI=ON
#             -DANDROID_ABI=arm64-v8a -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
# ============================================================================
set -euo pipefail
echo "[gen-android] This target will be plugged in at a later phase (the skeleton is ready for core/ reuse)."
echo "[gen-android] Requires: Android NDK r26+ and Gradle."
exit 0
