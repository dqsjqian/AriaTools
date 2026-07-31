# ============================================================================
#  ModuleTestProject.cmake — 单模块测试工程的共享装配。
#  被 modules/<mod>/tests/CMakeLists.txt include，拉起模块所需最小依赖：
#  aria + wb_utils + wb_infra + wb_module_api + 被测模块。
#
#  用法：
#    cmake -S Workbench/modules/notes/tests -B build/mac/modules/notes
#    cmake --build build/mac/modules/notes
#    ctest --test-dir build/mac/modules/notes --output-on-failure
#  模块测试产物必须归属目标平台：build/<platform>/modules/<module>。
# ============================================================================
# 期望调用方（tests/CMakeLists.txt）已定义：
#   WB_MOD_NAME     模块名（如 notes）
#   WB_MOD_DIR      模块根目录（modules/<mod>）

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)   # Workbench/
get_filename_component(_REPO_ROOT "${_WB_ROOT}/.." ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH ${_WB_ROOT}/cmake)
include(WbModule)

set(WB_RUNTIME_I18N_DIR "${CMAKE_BINARY_DIR}/i18n" CACHE INTERNAL "")
set(WB_RUNTIME_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets" CACHE INTERNAL "")

# 独立构建默认不带平台 UI（只验证 core 分层可编译）。可显式开 -DWORKBENCH_TARGET_QT=ON。
option(WORKBENCH_TARGET_QT "" OFF)
option(WORKBENCH_TARGET_IOS "" OFF)

# Aria（最小：不建测试/示例）。
set(ARIA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
if(WORKBENCH_TARGET_QT)
    set(ARIA_BUILD_QT6 ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(${_REPO_ROOT}/third_party/aria ${CMAKE_BINARY_DIR}/aria EXCLUDE_FROM_ALL)

# 基础库。
add_subdirectory(${_WB_ROOT}/core/utils      ${CMAKE_BINARY_DIR}/wb_utils)
add_subdirectory(${_WB_ROOT}/core/infra      ${CMAKE_BINARY_DIR}/wb_infra)
add_subdirectory(${_WB_ROOT}/core/module_api ${CMAKE_BINARY_DIR}/wb_module_api)

# 测试工程默认不编平台 View，仅验证模块 core。

# 本模块库。
add_subdirectory(${WB_MOD_DIR} ${CMAKE_BINARY_DIR}/wb_module_${WB_MOD_NAME})
