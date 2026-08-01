# ============================================================================
#  ModuleTestProject.cmake — shared assembly for single-module test projects.
#  Included by modules/<mod>/tests/CMakeLists.txt; pulls in the minimal dependencies needed by a module:
#  aria + wb_utils + wb_infra + wb_module_api + the module under test.
#
#  Usage:
#    cmake -S Workbench/modules/notes/tests -B build/mac/modules/notes
#    cmake --build build/mac/modules/notes
#    ctest --test-dir build/mac/modules/notes --output-on-failure
#  Module test artifacts must belong to the target platform: build/<platform>/modules/<module>.
# ============================================================================
# Expects the caller (tests/CMakeLists.txt) to have defined:
#   WB_MOD_NAME     module name (e.g. notes)
#   WB_MOD_DIR      module root directory (modules/<mod>)

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)   # Workbench/
get_filename_component(_REPO_ROOT "${_WB_ROOT}/.." ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH ${_WB_ROOT}/cmake)
include(WbModule)

set(WB_RUNTIME_I18N_DIR "${CMAKE_BINARY_DIR}/i18n" CACHE INTERNAL "")
set(WB_RUNTIME_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets" CACHE INTERNAL "")

# Standalone builds exclude platform UI by default (only verifying that the core layers compile). Enable explicitly with -DWORKBENCH_TARGET_QT=ON.
option(WORKBENCH_TARGET_QT "" OFF)
option(WORKBENCH_TARGET_IOS "" OFF)

# Aria (minimal: no tests/examples).
set(ARIA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
if(WORKBENCH_TARGET_QT)
    set(ARIA_BUILD_QT6 ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(${_REPO_ROOT}/third_party/aria ${CMAKE_BINARY_DIR}/aria EXCLUDE_FROM_ALL)

# Base libraries.
add_subdirectory(${_WB_ROOT}/core/utils      ${CMAKE_BINARY_DIR}/wb_utils)
add_subdirectory(${_WB_ROOT}/core/infra      ${CMAKE_BINARY_DIR}/wb_infra)
add_subdirectory(${_WB_ROOT}/core/module_api ${CMAKE_BINARY_DIR}/wb_module_api)

# Test projects do not build platform views by default; only the module core is verified.

# This module's library.
add_subdirectory(${WB_MOD_DIR} ${CMAKE_BINARY_DIR}/wb_module_${WB_MOD_NAME})
