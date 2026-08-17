wb_add_module(NAME theme
    SOURCES viewmodels/ThemeVm.cpp module/ThemeModule.cpp
    QT_SOURCES  platforms/qt/ThemeView.cpp
    IOS_SOURCES platforms/ios/ThemeView.mm
)
