# ============================================================================
#  wb_module_api — module contract library. The only cross-cutting dependency for modules (besides wb_platform + aria).
#  Contains: IModule / ModuleContext / ModuleRegistry / AppEvents / LocalizedVm.
# ============================================================================

# Navigation and mount contracts expose JSON even without Aria's HTTP adapter.
# Declare the dependency here so native and standalone builds do not depend on
# an unrelated adapter having already created this target.
if(NOT TARGET nlohmann_json::nlohmann_json)
    include("${ARIA_DIR}/cmake/ariaFetchPinned.cmake")
    aria_fetch_pinned_archive(
        NAME json
        VERSION 3.12.0
        URL "https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz"
        SHA256 42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    )
    add_library(wb_json INTERFACE)
    target_include_directories(wb_json SYSTEM INTERFACE
        "${ARIA_PINNED_JSON_SOURCE_DIR}/include")
    add_library(nlohmann_json::nlohmann_json ALIAS wb_json)
endif()

add_library(wb_module_api STATIC
    ModuleRegistry.cpp
)

target_include_directories(wb_module_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # include like "module_api/IModule.h"
    
)

target_link_libraries(wb_module_api PUBLIC
    wb_infra
    aria::binding
    aria::core

    nlohmann_json::nlohmann_json)

target_compile_features(wb_module_api PUBLIC cxx_std_23)
