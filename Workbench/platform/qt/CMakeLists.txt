# ============================================================================
#  Qt desktop: wb_qt_support (shared UI support) + workbench executable.
# ============================================================================
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)

# -- wb_qt_support: UiHelpers + QtViewFactory, shared by shell and module views --
add_library(wb_qt_support STATIC
    support/UiHelpers.cpp
    support/QtViewFactory.cpp
)
target_include_directories(wb_qt_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wb_qt_support PUBLIC aria::qt6 Qt6::Widgets)

# -- workbench executable --
add_executable(workbench WIN32 MACOSX_BUNDLE
    main.cpp
    App/QtAppShell.cpp
    App/QtViewManifest.cpp
)

target_include_directories(workbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(workbench PRIVATE
    wb_core_app          # app assembly (pulls in all module libs: viewmodels/services)
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

# -- macOS: copy i18n / assets into the .app bundle Resources so it is self-contained --
# See the matching comment in the top-level CMakeLists.txt; POST_BUILD copy must live
# in the same directory as the workbench target. The i18n content is guaranteed ready
# by the wb_i18n_* dependencies added to workbench in the top-level CMakeLists.txt.
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

# -- Windows: deploy runtime deps next to workbench.exe so it runs by double-click --
# Three things are needed:
#   1. workbench.exe must sit beside aria_*.dll (the Aria shared libs). Aria's CMake
#      puts those in ${CMAKE_BINARY_DIR}/bin/<config>/, so we mirror that for the exe.
#   2. Qt6 DLLs + the platforms/qwindows plugin are copied by windeployqt.
#   3. i18n resources go to <exe-dir>/i18n (QtAppShell::resolve_i18n_dir looks there).
# The VC++ runtime is assumed present (VS 2026 installs it system-wide); use
# --no-compiler-runtime to skip copying vcruntime/msvcp redist DLLs.
if(WIN32)
    set_target_properties(workbench PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    if(NOT WINDEPLOYQT_EXECUTABLE)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt
            HINTS "${Qt6_DIR}/../../../bin")
    endif()

    add_custom_command(TARGET workbench POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-translations --no-system-d3d-compiler
                --no-opengl-sw --no-compiler-runtime
                "$<TARGET_FILE:workbench>"
        VERBATIM)

    add_custom_command(TARGET workbench POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WB_RUNTIME_I18N_DIR}" "$<TARGET_FILE_DIR:workbench>/i18n"
        VERBATIM)
endif()
