wb_add_module(NAME sync
    SOURCES     viewmodels/SyncVm.cpp module/SyncModule.cpp
    QT_SOURCES  platforms/qt/SyncView.cpp
    IOS_SOURCES platforms/ios/SyncView.mm
)
