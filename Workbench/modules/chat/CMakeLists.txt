wb_add_module(NAME chat
    SOURCES viewmodels/ChatVm.cpp module/ChatModule.cpp
    QT_SOURCES  platforms/qt/ChatView.cpp
                platforms/qt/ChatViewEntry.cpp
    IOS_SOURCES platforms/ios/ChatView.mm platforms/ios/ChatViewEntry.mm
)
