wb_add_module(NAME search
    SOURCES viewmodels/SearchVm.cpp module/SearchModule.cpp
    QT_SOURCES  platforms/qt/SearchView.cpp
    IOS_SOURCES platforms/ios/SearchView.mm
)
