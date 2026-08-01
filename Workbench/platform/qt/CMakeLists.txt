# ============================================================================
#  Qt 桌面端：wb_qt_support（共享 UI 支撑）+ workbench 可执行程序。
# ============================================================================
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)

# ── wb_qt_support：UiHelpers + QtViewFactory，供 shell 与各模块 View 复用 ──
add_library(wb_qt_support STATIC
    support/UiHelpers.cpp
    support/QtViewFactory.cpp
)
target_include_directories(wb_qt_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wb_qt_support PUBLIC aria::qt6 Qt6::Widgets)

# ── workbench 可执行 ──
add_executable(workbench WIN32 MACOSX_BUNDLE
    main.cpp
    App/QtAppShell.cpp
    App/QtViewManifest.cpp
)

target_include_directories(workbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(workbench PRIVATE
    wb_core_app          # 应用装配（含各模块库，传递 viewmodels/domain/services）
    wb_qt_support
    aria::qt6
    Qt6::Widgets
)

set_target_properties(workbench PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME        "Workbench"
    MACOSX_BUNDLE_GUI_IDENTIFIER     "com.workbench.app"
    MACOSX_BUNDLE_BUNDLE_VERSION     "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
)

# ── 把运行期 i18n / assets 复制进 .app 包内 Resources，使 App 自包含 ────────
# 说明见顶层 CMakeLists 的同名注释；此处只做 POST_BUILD 复制（须与 workbench
# 目标同目录）。i18n 内容由顶层对 workbench 追加的 wb_i18n_* 依赖保证已就绪。
if(APPLE)
    set(_bundle_res "$<TARGET_BUNDLE_CONTENT_DIR:workbench>/Resources")
    add_custom_command(TARGET workbench POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WB_RUNTIME_I18N_DIR}" "${_bundle_res}/i18n"
        VERBATIM)
    add_custom_command(TARGET workbench POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${WB_RUNTIME_ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WB_RUNTIME_ASSETS_DIR}" "${_bundle_res}/assets"
        VERBATIM)
endif()
