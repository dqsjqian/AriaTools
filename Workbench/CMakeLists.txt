# ============================================================================
#  Workbench — cross-platform workbench (Aria C++ MVVM flagship example)
#
#  Architecture:
#    third_party/aria   vendored framework
#    core/utils         wb_utils      pure functions + platform macros
#    core/infra         wb_infra      stable services (i18n/storage/secret/crypto/settings/sync/log)
#                                     + DI container + EventBus
#    core/module_api    wb_module_api module contract (IModule/Context/Registry/BaseVm/AppEvents)
#    core/app           wb_core_app   assembly layer (iterates module registry + ModulesManifest)
#    modules/<mod>      wb_module_<m> one module per lib (models/services/viewmodels/module/platforms/assets/tests)
#    platform/qt|ios    thin per-platform shells (entry + shell + support)
#
#  One core, multiple view shells; modules build independently. Platform switches:
#    -DWORKBENCH_TARGET_QT=ON       Qt desktop (default)
#    -DWORKBENCH_TARGET_IOS=ON      iOS UIKit (requires -G Xcode -DCMAKE_SYSTEM_NAME=iOS)
#    -DWORKBENCH_TARGET_ANDROID=ON  Android: builds the pure-C++ core + module
#                                   static libraries only; the Kotlin/Compose
#                                   view shell + JNI bridge live in
#                                   platform/android/app (Gradle), which links
#                                   the .a archives produced here.
# ============================================================================
cmake_minimum_required(VERSION 3.20)
project(workbench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MSVC defaults to the system code page for source files without a BOM. Force UTF-8
# so the compiler never misinterprets byte sequences in third-party headers or future
# contributions. Equivalent to /source-charset:utf-8 /execution-charset:utf-8;
# a no-op for Clang/GCC. (Workbench's own sources are ASCII/English-only by convention;
# this flag is a defensive default, not a workaround for non-ASCII source.)
if(MSVC)
    add_compile_options(/utf-8)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(WbModule)

# Runtime resource sink directories. Each module still owns its assets/ tree.
set(WB_RUNTIME_I18N_DIR "${CMAKE_BINARY_DIR}/i18n" CACHE INTERNAL "")
set(WB_RUNTIME_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets" CACHE INTERNAL "")

option(WORKBENCH_TARGET_QT       "Build the Qt6 desktop app (Win/Mac/Linux)" ON)
option(WORKBENCH_TARGET_IOS      "Build the iOS UIKit app"                    OFF)
option(WORKBENCH_TARGET_ANDROID  "Build the Android JNI core (static libs)"   OFF)

if(WORKBENCH_TARGET_IOS)
    set(WORKBENCH_TARGET_QT OFF CACHE BOOL "" FORCE)
    enable_language(OBJCXX)
endif()
if(WORKBENCH_TARGET_ANDROID)
    set(WORKBENCH_TARGET_QT OFF CACHE BOOL "" FORCE)
endif()

# -- Pull in the Aria framework --
set(ARIA_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(ARIA_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
if(WORKBENCH_TARGET_QT)
    set(ARIA_BUILD_QT6 ON CACHE BOOL "" FORCE)
endif()
if(WORKBENCH_TARGET_IOS)
    set(ARIA_BUILD_UIKIT ON CACHE BOOL "" FORCE)
    set(ARIA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
endif()
if(WORKBENCH_TARGET_ANDROID)
    # Android consumers link STATIC archives; the JNI adapter bridges the
    # core to Kotlin/Compose (View layer lives in platform/android/app).
    set(ARIA_BUILD_JNI   ON  CACHE BOOL "" FORCE)
    set(ARIA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(ARIA_BUILD_QT6   OFF CACHE BOOL "" FORCE)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/aria
                 ${CMAKE_BINARY_DIR}/aria
                 EXCLUDE_FROM_ALL)

# -- Base libraries (utils/infra/module_api) --
add_subdirectory(core/utils)
add_subdirectory(core/infra)
add_subdirectory(core/module_api)

# -- Qt support (shared by module platforms/qt views and the shell; must come before modules) --
if(WORKBENCH_TARGET_QT)
    # wb_qt_support is defined in platform/qt/CMakeLists.txt, but modules need it -> add the subdir early.
endif()

# -- Business modules (one lib each). Collect lib names for wb_core_app to link. --
set(WB_MODULES dashboard notes calendar tools settings sync
              tipcalc unitconvert cart signup search login chat theme wizard)
set(WB_MODULE_LIBS "")
foreach(m ${WB_MODULES})
    list(APPEND WB_MODULE_LIBS wb_module_${m})
endforeach()
set(WB_MODULE_LIBS ${WB_MODULE_LIBS} CACHE INTERNAL "")

# platform/qt must provide wb_qt_support first (module platforms/qt depend on it).
if(WORKBENCH_TARGET_QT)
    add_subdirectory(platform/qt)     # defines wb_qt_support + workbench executable
endif()
if(WORKBENCH_TARGET_IOS)
    add_subdirectory(platform/ios)    # defines wb_ios_support (if any)
endif()

foreach(m ${WB_MODULES})
    add_subdirectory(modules/${m})
endforeach()

# -- App assembly layer (depends on all module libs) --
add_subdirectory(core/app)

# -- Copy shared i18n ("common" namespace) to the runtime directory --
file(GLOB _common_i18n ${CMAKE_CURRENT_SOURCE_DIR}/modules/_shared/assets/i18n/*.xml)
add_custom_target(wb_i18n_common ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WB_RUNTIME_I18N_DIR}/common"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_common_i18n} "${WB_RUNTIME_I18N_DIR}/common"
    VERBATIM)

# -- Stage i18n / assets into the macOS .app bundle so the app is self-contained --
# Background: QtAppShell::resolve_i18n_dir looks first for i18n/ beside the executable,
# then <bundle>/Contents/Resources/i18n. Runtime strings are gathered into
# ${WB_RUNTIME_I18N_DIR} (= build/<plat>/i18n) by the wb_i18n_* targets, but that dir
# is outside the .app bundle; if the user launches via `open workbench.app` (working
# directory = /), the old logic failed to find the strings and the UI showed [module/key]
# placeholders instead of text. The actual POST_BUILD copy lives in platform/qt (same
# dir as the workbench target, as CMake requires); here we only add the dependency so
# the content is ready before the copy runs. Both Qt and iOS need this (iOS does its
# POST_BUILD copy in platform/ios/CMakeLists.txt).
if(TARGET workbench)
    add_dependencies(workbench wb_i18n_common)
    foreach(_m ${WB_MODULES})
        if(TARGET wb_i18n_${_m})
            add_dependencies(workbench wb_i18n_${_m})
        endif()
    endforeach()
endif()

# -- Patch the vendored UIKit adapter include path (bare #import issue) --
if(WORKBENCH_TARGET_IOS AND TARGET aria_adapters_uikit)
    target_include_directories(aria_adapters_uikit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/aria/modules/adapters/uikit/include/aria/adapters/uikit)
endif()
