# ============================================================================
#  wb_module_api — 模块契约库。模块唯一的横向依赖（除 wb_platform + aria）。
#  含：IModule / ModuleContext / ModuleRegistry / AppEvents / LocalizedVm。
# ============================================================================
add_library(wb_module_api STATIC
    ModuleRegistry.cpp
)

target_include_directories(wb_module_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # include 形如 "module_api/IModule.h"
)

target_link_libraries(wb_module_api PUBLIC
    wb_infra
    aria::binding
    aria::core
)

target_compile_features(wb_module_api PUBLIC cxx_std_20)
