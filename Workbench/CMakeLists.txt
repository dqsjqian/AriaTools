# ============================================================================
#  Workbench — 跨平台工作台（Aria C++ MVVM 标杆案例）
#
#  架构：
#    third_party/aria   vendored 框架
#    core/utils         wb_utils      纯函数 + 平台宏
#    core/infra         wb_infra      稳定服务层(i18n/storage/secret/crypto/settings/sync/log)
#                                     + DI Container + EventBus
#    core/module_api    wb_module_api 模块契约(IModule/Context/Registry/LocalizedVm/AppEvents)
#    core/app           wb_core_app   装配层(遍历模块注册表 + ModulesManifest)
#    modules/<mod>      wb_module_<m> 一模块一库(models/services/viewmodels/module/platforms/assets/tests)
#    platform/qt|ios    各端薄壳(入口 + shell + support)
#
#  一份 core，多端 View；模块可独立构建。平台开关：
#    -DWORKBENCH_TARGET_QT=ON    Qt 桌面(默认)
#    -DWORKBENCH_TARGET_IOS=ON   iOS UIKit(需 -G Xcode -DCMAKE_SYSTEM_NAME=iOS)
# ============================================================================
cmake_minimum_required(VERSION 3.20)
project(workbench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(WbModule)

# 运行时资源汇聚目录：各模块仍在自身 assets/ 下维护资源。
set(WB_RUNTIME_I18N_DIR "${CMAKE_BINARY_DIR}/i18n" CACHE INTERNAL "")
set(WB_RUNTIME_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets" CACHE INTERNAL "")

option(WORKBENCH_TARGET_QT  "Build the Qt6 desktop app (Win/Mac/Linux)" ON)
option(WORKBENCH_TARGET_IOS "Build the iOS UIKit app"                    OFF)

if(WORKBENCH_TARGET_IOS)
    set(WORKBENCH_TARGET_QT OFF CACHE BOOL "" FORCE)
    enable_language(OBJCXX)
endif()

# ── 引入 Aria 框架 ─────────────────────────────────────────────────────────
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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/aria
                 ${CMAKE_BINARY_DIR}/aria
                 EXCLUDE_FROM_ALL)

# ── 基础库（utils/infra/module_api）──────────────────────────────────────
add_subdirectory(core/utils)
add_subdirectory(core/infra)
add_subdirectory(core/module_api)

# ── Qt support（供各模块 platforms/qt 与外壳复用；必须在模块之前）──────────
if(WORKBENCH_TARGET_QT)
    # wb_qt_support 定义在 platform/qt/CMakeLists.txt，但模块需要它 → 提前加子目录
endif()

# ── 各业务模块（一模块一库）。收集库名给 wb_core_app 链接 ──────────────────
set(WB_MODULES dashboard notes calendar tools settings sync)
set(WB_MODULE_LIBS "")
foreach(m ${WB_MODULES})
    list(APPEND WB_MODULE_LIBS wb_module_${m})
endforeach()
set(WB_MODULE_LIBS ${WB_MODULE_LIBS} CACHE INTERNAL "")

# platform/qt 需先提供 wb_qt_support（模块 platforms/qt 依赖它）。
if(WORKBENCH_TARGET_QT)
    add_subdirectory(platform/qt)     # 定义 wb_qt_support + workbench 可执行
endif()
if(WORKBENCH_TARGET_IOS)
    add_subdirectory(platform/ios)    # 定义 wb_ios_support（若有）
endif()

foreach(m ${WB_MODULES})
    add_subdirectory(modules/${m})
endforeach()

# ── 应用装配层（依赖各模块库）──────────────────────────────────────────────
add_subdirectory(core/app)

# ── 公共 i18n（common）拷到运行目录 ────────────────────────────────────────
file(GLOB _common_i18n ${CMAKE_CURRENT_SOURCE_DIR}/modules/_shared/assets/i18n/*.xml)
add_custom_target(wb_i18n_common ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WB_RUNTIME_I18N_DIR}/common"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_common_i18n} "${WB_RUNTIME_I18N_DIR}/common"
    VERBATIM)

# ── 把运行期 i18n / assets 打进 macOS .app 包，使 App 自包含 ────────────────
# 现象背景：QtAppShell::resolve_i18n_dir 会先找可执行文件旁的 i18n/，其次
# <bundle>/Contents/Resources/i18n。运行期文案先由各 wb_i18n_* 目标汇聚到
# ${WB_RUNTIME_I18N_DIR}（= build/<plat>/i18n），但它在 .app 包外；若用户用
# `open workbench.app` 启动（工作目录为 /），旧逻辑找不到文案，界面会显示
# [module/key] 而非具体文本。实际的 POST_BUILD 复制放在 platform/qt（与
# workbench 目标同目录，CMake 要求）；这里只补依赖，确保内容已就绪再复制。
if(APPLE AND WORKBENCH_TARGET_QT AND TARGET workbench)
    add_dependencies(workbench wb_i18n_common)
    foreach(_m ${WB_MODULES})
        if(TARGET wb_i18n_${_m})
            add_dependencies(workbench wb_i18n_${_m})
        endif()
    endforeach()
endif()

# ── 修补 vendored UIKit 适配器的头文件搜索路径（裸 #import 问题）──────────────
if(WORKBENCH_TARGET_IOS AND TARGET aria_adapters_uikit)
    target_include_directories(aria_adapters_uikit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/aria/modules/adapters/uikit/include/aria/adapters/uikit)
endif()
