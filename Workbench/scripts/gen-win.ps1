# ============================================================================
#  gen-win.ps1 — 生成 Windows (Qt6) 工程  【后续阶段接入】
#
#  规划：Windows 与 Mac 共用 platform/qt/ 的 View 层，仅工具链不同。
#    cmake -S Workbench -B build/win -G "Visual Studio 17 2022" `
#      -DWORKBENCH_TARGET_QT=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.x/msvc2022_64"
#    cmake --build build/win --config Release
# ============================================================================
Write-Host "[gen-win] 该端将在后续阶段接入（与 Mac 共用 platform/qt/ View）。"
Write-Host "[gen-win] 需要：Visual Studio 2022 + Qt6 (msvc2022_64)。"
exit 0
