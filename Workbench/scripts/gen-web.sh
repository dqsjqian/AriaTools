#!/usr/bin/env bash
# ============================================================================
#  gen-web.sh — 生成 Web (HTTP/REST/SSE 适配器) 工程  【后续阶段接入】
#
#  规划：Aria 的 Web 是「C++ 后端把 ViewModel 经 REST+SSE 暴露 + 浏览器瘦客户端」。
#    - 复用 core/ 的 VM
#    - 用 aria::http 适配器起服务，浏览器端用 aria_client.js
#    - CMake: -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_HTTP=ON
# ============================================================================
set -euo pipefail
echo "[gen-web] 该端将在后续阶段接入（core/ VM 经 aria::http 暴露为 REST+SSE）。"
echo "[gen-web] 计划：cmake -S Workbench -B build/web -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_HTTP=ON"
exit 0
