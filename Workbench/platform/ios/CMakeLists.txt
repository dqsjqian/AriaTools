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

set_target_properties(workbench PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.workbench.app"
    MACOSX_BUNDLE_BUNDLE_NAME "Workbench"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.workbench.app"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
)
