#!/usr/bin/env bash
# ============================================================================
#  gen-android.sh — 生成 Android (JNI) Gradle 工程  【后续阶段接入】
#
#  规划：
#    - 复用 core/ 静态库（NDK r26+ 交叉编译）
#    - View 层用原生 Kotlin/Compose + aria::adapters::jni
#    - CMake: -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_JNI=ON
#             -DANDROID_ABI=arm64-v8a -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
# ============================================================================
set -euo pipefail
echo "[gen-android] 该端将在后续阶段接入（骨架已为 core/ 复用做好准备）。"
echo "[gen-android] 需要：Android NDK r26+ 与 Gradle。"
exit 0
