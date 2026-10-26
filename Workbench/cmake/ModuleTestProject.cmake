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

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Same MSVC source-encoding guard as the top-level Workbench/CMakeLists.txt:
# without /utf-8, sources containing Chinese comments are read as the system
# code page (GBK) and multibyte sequences can swallow a closing quote/newline
# (C2001/C2143). The main build adds this globally; standalone module test
# projects must too.
if(MSVC)
    add_compile_options(/utf-8)
endif()

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)   # Workbench/
get_filename_component(_REPO_ROOT "${_WB_ROOT}/.." ABSOLUTE)

set(ARIA_DEPENDENCIES_FILE "${_REPO_ROOT}/dependencies.json" CACHE FILEPATH
    "Shared dependency requests and resolved versions")

list(APPEND CMAKE_MODULE_PATH "${_WB_ROOT}/cmake")
include(WbModule)

set(WB_RUNTIME_I18N_DIR "${CMAKE_BINARY_DIR}/i18n" CACHE INTERNAL "")
set(WB_RUNTIME_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets" CACHE INTERNAL "")

# Standalone builds exclude platform UI by default (only verifying that the core layers compile). Enable explicitly with -DWORKBENCH_TARGET_QT=ON.
option(WORKBENCH_TARGET_QT "" OFF)
option(WORKBENCH_TARGET_IOS "" OFF)

set(ARIA_DIR "" CACHE PATH "Optional editable Aria source for the shared SDK recipe")
set(ARIA_SDK_SOURCE "${ARIA_DIR}")
option(ARIA_SDK_QT "Shared SDK Qt adapter" ${WORKBENCH_TARGET_QT})
option(ARIA_SDK_HTTP "Shared SDK HTTP adapter" OFF)
option(ARIA_SDK_TLS "Shared SDK HTTP TLS" OFF)
option(ARIA_SDK_JNI "Shared SDK JNI adapter" OFF)
option(ARIA_SDK_UIKIT "Shared SDK UIKit adapter" ${WORKBENCH_TARGET_IOS})
option(ARIA_SDK_SHARED "Build shared Aria libraries" OFF)
include(SharedAria)

# Base libraries.
add_subdirectory("${_WB_ROOT}/core/utils" "${CMAKE_BINARY_DIR}/wb_utils")
add_subdirectory("${_WB_ROOT}/core/infra" "${CMAKE_BINARY_DIR}/wb_infra")
add_subdirectory("${_WB_ROOT}/core/module_api" "${CMAKE_BINARY_DIR}/wb_module_api")

# Test projects do not build platform views by default; only the module core is verified.

# This module's library.
add_subdirectory("${WB_MOD_DIR}" "${CMAKE_BINARY_DIR}/wb_module_${WB_MOD_NAME}")
