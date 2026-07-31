wb_add_module(NAME settings
    SOURCES     viewmodels/SettingsVm.cpp module/SettingsModule.cpp
    QT_SOURCES  platforms/qt/SettingsView.cpp
    IOS_SOURCES platforms/ios/SettingsView.mm
)
