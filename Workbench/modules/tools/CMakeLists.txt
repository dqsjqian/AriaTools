wb_add_module(NAME tools
    SOURCES
        services/ToolsService.cpp
        models/ToolsModel.cpp
        viewmodels/ToolsVm.cpp
        module/ToolsModule.cpp
    QT_SOURCES  platforms/qt/ToolsView.cpp
    IOS_SOURCES platforms/ios/ToolsView.mm
)

target_include_directories(wb_module_tools PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/aria/third_party/nlohmann_json/include
)
