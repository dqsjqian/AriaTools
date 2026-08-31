add_executable(workbench_web main.cpp)

# Align with the Qt shell: drop the exe into ${CMAKE_BINARY_DIR}/bin so it sits
# beside aria_*.dll (Aria's CMake already puts those in bin/). Without this,
# CMake's default per-target layout leaves the binary in
# build/web/platform/web/<Config>/ while the DLLs go to build/web/bin/<Config>/
# — the win build has no such split. Works for single- and multi-config generators
# alike (the latter append a per-config subdirectory).
set_target_properties(workbench_web PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
