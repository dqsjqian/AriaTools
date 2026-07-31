# wb_core_app — 应用装配层（AppCore 遍历模块 + ModulesManifest）。无平台 UI 依赖。
add_library(wb_core_app STATIC
    AppCore.cpp
    ModulesManifest.cpp
)

target_include_directories(wb_core_app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..       # "app/..", "infra/..", "module_api/.."
)

target_link_libraries(wb_core_app PUBLIC
    wb_infra
    wb_module_api
    wb_utils
    ${WB_MODULE_LIBS}
)

target_compile_features(wb_core_app PUBLIC cxx_std_20)
