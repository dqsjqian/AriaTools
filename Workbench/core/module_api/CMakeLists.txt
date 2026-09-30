# ============================================================================
#  wb_module_api — module contract library. The only cross-cutting dependency for modules (besides wb_platform + aria).
#  Contains: IModule / ModuleContext / ModuleRegistry / AppEvents / LocalizedVm.
# ============================================================================

# Navigation and mount contracts expose JSON even without Aria's HTTP adapter.
# Declare the dependency here so native and standalone builds do not depend on
# an unrelated adapter having already created this target.
if(NOT TARGET nlohmann_json::nlohmann_json)
    include("${ARIA_DIR}/cmake/ariaFetchPinned.cmake")
    aria_fetch_pinned_archive(NAME json)
    add_library(wb_json INTERFACE)
    target_include_directories(wb_json SYSTEM INTERFACE
        "${ARIA_PINNED_JSON_SOURCE_DIR}/include")
    add_library(nlohmann_json::nlohmann_json ALIAS wb_json)
endif()

if(WORKBENCH_TARGET_ANDROID)
    # The JNI bridge is a separate CMake project. Export the headers selected
    # by the actual target, including parent targets and source overrides;
    # searching a shared cache can silently pick a different JSON version.
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/wb-json-includes-$<CONFIG>.txt"
        CONTENT "$<JOIN:$<TARGET_PROPERTY:nlohmann_json::nlohmann_json,INTERFACE_INCLUDE_DIRECTORIES>,\n>\n")
    add_custom_target(wb_stage_json_headers ALL
        COMMAND ${CMAKE_COMMAND}
            "-DWB_JSON_MANIFEST=${CMAKE_BINARY_DIR}/wb-json-includes-$<CONFIG>.txt"
            "-DWB_JSON_DESTINATION=${CMAKE_BINARY_DIR}/include/nlohmann"
            -P "${CMAKE_CURRENT_LIST_DIR}/../../cmake/StageJsonHeaders.cmake"
        VERBATIM)
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
