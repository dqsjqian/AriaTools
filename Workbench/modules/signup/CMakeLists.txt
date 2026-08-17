wb_add_module(NAME signup
    SOURCES viewmodels/SignupVm.cpp module/SignupModule.cpp
    QT_SOURCES  platforms/qt/SignupView.cpp
    IOS_SOURCES platforms/ios/SignupView.mm
)
