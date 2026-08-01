# ============================================================================
#  iOS: wb_ios_support (shared UIKit support) + workbench app.
# ============================================================================

# ── wb_ios_support: UIViewFactory + IosUi, reused by shell and module iOS views ──
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

# Bundle ID: a free personal Team requires global uniqueness, so default to a value with a personal suffix;
#   override with -DWB_IOS_BUNDLE_ID=... .
set(WB_IOS_BUNDLE_ID "com.dqsjqian.workbench" CACHE STRING "iOS bundle identifier")

set_target_properties(workbench PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${WB_IOS_BUNDLE_ID}"
    MACOSX_BUNDLE_BUNDLE_NAME "Workbench"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${WB_IOS_BUNDLE_ID}"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
)

# Signing: device builds must be signed; simulator builds can skip signing.
#   Pass -DWB_IOS_DEV_TEAM=<TeamID> → Xcode automatic signing (device).
#   Omit                            → signing disabled (simulator only).
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

# Pack runtime i18n (and assets) into the iOS .app bundle so the app is self-contained.
# IosShell::bundle_i18n_dir looks up strings under <Bundle>/i18n; these xml files are first
# gathered by each wb_i18n_* target into ${WB_RUNTIME_I18N_DIR} (the top level already makes
# workbench depend on them), then copied into the bundle after linking. Otherwise the runtime
# cannot find strings and the UI displays [module/key].
#
# Use the Xcode POST_BUILD environment variables $TARGET_BUILD_DIR/$FULL_PRODUCT_NAME to
# locate the .app (cannot use $<TARGET_BUNDLE_CONTENT_DIR>: under the Xcode generator it
# contains an unreplaced ${EFFECTIVE_PLATFORM_NAME}). Use cmake -E copy_directory
# (idempotent, cross-platform) without VERBATIM, so $TARGET_BUILD_DIR etc. are expanded by
# Xcode's shell.
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
