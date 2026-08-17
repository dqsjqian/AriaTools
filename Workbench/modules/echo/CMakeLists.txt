wb_add_module(NAME echo
    SOURCES module/EchoModule.cpp
    QT_SOURCES  platforms/qt/EchoView.cpp
                platforms/qt/EchoViewEntry.cpp
    IOS_SOURCES platforms/ios/EchoView.mm platforms/ios/EchoViewEntry.mm
)
