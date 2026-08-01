# ============================================================================
#  iOS 端：wb_ios_support（共享 UIKit 支撑）+ workbench app。
# ============================================================================

# ── wb_ios_support：UIViewFactory + IosUi，供 shell 与各模块 ios View 复用 ──
add_library(wb_ios_support STATIC
    support/UIViewFactory.mm
    support/IosUi.mm
)
target_include_directories(wb_ios_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wb_ios_support PUBLIC
    aria::adapters::uikit
    "-framework UIKit"
    "-framework Foundation")
target_compile_options(wb_ios_support PRIVATE -fobjc-arc)

# ── workbench app ──
add_executable(workbench MACOSX_BUNDLE
    App/main.mm
    App/AppDelegate.mm
    App/SceneDelegate.mm
    App/IosShell.mm
    App/IosViewManifest.mm
)
target_include_directories(workbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(workbench PRIVATE
    wb_core_app
    wb_ios_support
    aria::adapters::uikit
    "-framework UIKit"
    "-framework Foundation")
target_compile_options(workbench PRIVATE -fobjc-arc)

# Bundle ID：免费个人 Team 需全局唯一，故用带个人后缀的默认值，可用
#   -DWB_IOS_BUNDLE_ID=... 覆盖。
set(WB_IOS_BUNDLE_ID "com.dqsjqian.workbench" CACHE STRING "iOS bundle identifier")

set_target_properties(workbench PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${WB_IOS_BUNDLE_ID}"
    MACOSX_BUNDLE_BUNDLE_NAME "Workbench"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${WB_IOS_BUNDLE_ID}"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
)

# 签名：真机必须签名，模拟器可免签名。
#   传入 -DWB_IOS_DEV_TEAM=<TeamID> → Xcode 自动签名（真机）。
#   不传                              → 关闭签名（仅模拟器）。
if(WB_IOS_DEV_TEAM)
    set_target_properties(workbench PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "YES"
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${WB_IOS_DEV_TEAM}"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Apple Development")
else()
    set_target_properties(workbench PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
endif()

# 把运行期 i18n（及 assets）打进 iOS .app bundle，使 App 自包含。
# IosShell::bundle_i18n_dir 会在 <Bundle>/i18n 查找文案；这些 xml 先由各
# wb_i18n_* 目标汇聚到 ${WB_RUNTIME_I18N_DIR}（顶层已让 workbench 依赖它们），
# 这里在链接后复制进 bundle。否则运行时找不到文案，界面显示 [module/key]。
#
# 用 Xcode POST_BUILD 环境变量 $TARGET_BUILD_DIR/$FULL_PRODUCT_NAME 定位 .app
# （不能用 $<TARGET_BUNDLE_CONTENT_DIR>，在 Xcode 生成器下含未替换的
# ${EFFECTIVE_PLATFORM_NAME}）。用 cmake -E copy_directory（幂等、跨平台），
# 不加 VERBATIM，使 $TARGET_BUILD_DIR 等变量交由 Xcode 的 shell 展开。
add_custom_command(TARGET workbench POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${WB_RUNTIME_I18N_DIR}"
            "$TARGET_BUILD_DIR/$FULL_PRODUCT_NAME/i18n")
if(EXISTS "${WB_RUNTIME_ASSETS_DIR}")
    add_custom_command(TARGET workbench POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WB_RUNTIME_ASSETS_DIR}"
                "$TARGET_BUILD_DIR/$FULL_PRODUCT_NAME/assets")
endif()
