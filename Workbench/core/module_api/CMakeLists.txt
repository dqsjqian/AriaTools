# ============================================================================
#  wb_module_api — module contract library. The only cross-cutting dependency for modules (besides wb_platform + aria).
#  Contains: IModule / ModuleContext / ModuleRegistry / AppEvents / LocalizedVm.
# ============================================================================
add_library(wb_module_api STATIC
    ModuleRegistry.cpp
)

target_include_directories(wb_module_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # include like "module_api/IModule.h"
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/aria/third_party/nlohmann_json/include
)

target_link_libraries(wb_module_api PUBLIC
    wb_infra
    aria::binding
    aria::core
)

target_compile_features(wb_module_api PUBLIC cxx_std_20)
