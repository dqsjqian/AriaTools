wb_add_module(NAME search
    SOURCES viewmodels/SearchVm.cpp module/SearchModule.cpp
    QT_SOURCES  platforms/qt/SearchView.cpp
                platforms/qt/SearchViewEntry.cpp
    IOS_SOURCES platforms/ios/SearchView.mm platforms/ios/SearchViewEntry.mm
)
