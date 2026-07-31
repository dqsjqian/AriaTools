# ============================================================================
#  WbModule.cmake — 定义 wb_add_module()，封装"一模块一库"的样板。
#  让每个 modules/<mod>/CMakeLists.txt 只需一行声明，保持标杆级简洁一致。
#
#  用法：
#    wb_add_module(NAME dashboard
#        SOURCES   viewmodels/DashboardVm.cpp module/DashboardModule.cpp
#        QT_SOURCES platforms/qt/DashboardView.cpp   # 可选
#    )
#  产出静态库 wb_module_<name>，include 根 = 模块目录（"models/..","services/..","viewmodels/.."）。
#  自动：链接 wb_module_api/wb_infra/wb_utils/aria；Qt 目标时编入 QT_SOURCES 并链 wb_qt_support；
#        把 assets/i18n/*.xml 汇聚到 ${WB_RUNTIME_I18N_DIR}/<name>/；
#        把 assets/icons/ 汇聚到 ${WB_RUNTIME_ASSETS_DIR}/<name>/icons/。
# ============================================================================
function(wb_add_module)
    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES QT_SOURCES IOS_SOURCES EXTRA_LIBS)
    cmake_parse_arguments(WBM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_tgt wb_module_${WBM_NAME})
    add_library(${_tgt} STATIC ${WBM_SOURCES})

    target_include_directories(${_tgt} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
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

    # 模块 i18n 资源汇聚到运行目录。
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

    # 模块图片等静态资源按模块命名空间汇聚；没有 icons/ 时不创建空目录。
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
