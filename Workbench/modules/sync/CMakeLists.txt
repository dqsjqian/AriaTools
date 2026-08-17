wb_add_module(NAME sync
    SOURCES     viewmodels/SyncVm.cpp module/SyncModule.cpp
    QT_SOURCES  platforms/qt/SyncView.cpp
                platforms/qt/SyncViewEntry.cpp
    IOS_SOURCES platforms/ios/SyncView.mm platforms/ios/SyncViewEntry.mm
)
