wb_add_module(NAME signup
    SOURCES viewmodels/SignupVm.cpp module/SignupModule.cpp
    QT_SOURCES  platforms/qt/SignupView.cpp
                platforms/qt/SignupViewEntry.cpp
    IOS_SOURCES platforms/ios/SignupView.mm platforms/ios/SignupViewEntry.mm
)
