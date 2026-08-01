#!/usr/bin/env bash
# ============================================================================
#  gen-web.sh — Generate the Web (HTTP/REST/SSE adapter) project  [plugged in at a later phase]
#
#  Plan: Aria's Web is a "C++ backend exposing the ViewModel via REST+SSE + a thin browser client".
#    - Reuse the core/ VM
#    - Serve via the aria::http adapter; the browser side uses aria_client.js
#    - CMake: -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_HTTP=ON
# ============================================================================
set -euo pipefail
echo "[gen-web] This target will be plugged in at a later phase (core/ VM exposed as REST+SSE via aria::http)."
echo "[gen-web] Plan: cmake -S Workbench -B build/web -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_HTTP=ON"
exit 0
