wb_add_module(NAME login
    SOURCES viewmodels/LoginVm.cpp module/LoginModule.cpp
    QT_SOURCES  platforms/qt/LoginView.cpp
                platforms/qt/LoginViewEntry.cpp
    IOS_SOURCES platforms/ios/LoginView.mm platforms/ios/LoginViewEntry.mm
)
