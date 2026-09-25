wb_add_module(NAME tools
    SOURCES
        services/ToolsService.cpp
        models/ToolsModel.cpp
        viewmodels/ToolsVm.cpp
        module/ToolsModule.cpp
    QT_SOURCES  platforms/qt/ToolsView.cpp
                platforms/qt/ToolsViewEntry.cpp
    IOS_SOURCES platforms/ios/ToolsView.mm platforms/ios/ToolsViewEntry.mm
)

target_link_libraries(wb_module_tools PRIVATE nlohmann_json::nlohmann_json)
