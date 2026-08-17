# ============================================================================
#  WbModule.cmake — defines wb_add_module(), encapsulating the "one module, one library" boilerplate.
#  Lets each modules/<mod>/CMakeLists.txt declare itself in a single line, keeping best-in-class simplicity and consistency.
#
#  Usage:
#    wb_add_module(NAME dashboard
#        SOURCES   viewmodels/DashboardVm.cpp module/DashboardModule.cpp
#        QT_SOURCES platforms/qt/DashboardView.cpp   # optional
#    )
#  Produces static library wb_module_<name>, include root = module directory ("models/..","services/..","viewmodels/..").
#  Automatic: links wb_module_api/wb_infra/wb_utils/aria; for Qt targets compiles QT_SOURCES and links wb_qt_support;
#        gathers assets/i18n/*.xml into ${WB_RUNTIME_I18N_DIR}/<name>/;
#        gathers assets/icons/ into ${WB_RUNTIME_ASSETS_DIR}/<name>/icons/.
# ============================================================================
function(wb_add_module)
    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES QT_SOURCES IOS_SOURCES EXTRA_LIBS)
    cmake_parse_arguments(WBM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_tgt wb_module_${WBM_NAME})
    add_library(${_tgt} STATIC ${WBM_SOURCES})

    target_include_directories(${_tgt} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../_shared)  # cross-module shared events/types
    target_link_libraries(${_tgt} PUBLIC
        wb_module_api wb_infra wb_utils aria::binding aria::core)
    if(WBM_EXTRA_LIBS)
        target_link_libraries(${_tgt} PUBLIC ${WBM_EXTRA_LIBS})
    endif()
    target_compile_features(${_tgt} PUBLIC cxx_std_20)

    if(WORKBENCH_TARGET_QT AND WBM_QT_SOURCES)
        target_sources(${_tgt} PRIVATE ${WBM_QT_SOURCES})
        target_link_libraries(${_tgt} PUBLIC wb_qt_support)
    endif()
    if(WORKBENCH_TARGET_IOS AND WBM_IOS_SOURCES)
        target_sources(${_tgt} PRIVATE ${WBM_IOS_SOURCES})
        target_link_libraries(${_tgt} PUBLIC wb_ios_support)
        set_source_files_properties(${WBM_IOS_SOURCES} PROPERTIES
            COMPILE_OPTIONS "-fobjc-arc")
    endif()

    # Gather module i18n resources into the runtime directory.
    if(WB_RUNTIME_I18N_DIR)
        file(GLOB _i18n ${CMAKE_CURRENT_SOURCE_DIR}/assets/i18n/*.xml)
        if(_i18n)
            add_custom_target(wb_i18n_${WBM_NAME} ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory "${WB_RUNTIME_I18N_DIR}/${WBM_NAME}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_i18n} "${WB_RUNTIME_I18N_DIR}/${WBM_NAME}"
                VERBATIM)
            add_dependencies(${_tgt} wb_i18n_${WBM_NAME})
        endif()
    endif()

    # Gather module images and other static resources under the module namespace; do not create an empty directory when icons/ is absent.
    set(_icons_dir ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons)
    if(WB_RUNTIME_ASSETS_DIR AND IS_DIRECTORY ${_icons_dir})
        add_custom_target(wb_assets_${WBM_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "${WB_RUNTIME_ASSETS_DIR}/${WBM_NAME}/icons"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${_icons_dir}"
                    "${WB_RUNTIME_ASSETS_DIR}/${WBM_NAME}/icons"
            VERBATIM)
        add_dependencies(${_tgt} wb_assets_${WBM_NAME})
    endif()
endfunction()
