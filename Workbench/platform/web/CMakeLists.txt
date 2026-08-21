add_executable(workbench_web main.cpp)

target_link_libraries(workbench_web PRIVATE
    wb_core_app
    wb_module_tipcalc
    aria::http
    aria::runtime
    aria::binding
)

target_compile_features(workbench_web PRIVATE cxx_std_20)

target_compile_definitions(workbench_web PRIVATE
    WB_WEB_STATIC_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/static"
    WB_WEB_I18N_ROOT="${WB_RUNTIME_I18N_DIR}"
)
