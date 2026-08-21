wb_add_module(NAME frameworklab
    SOURCES     viewmodels/FrameworkLabVm.cpp module/FrameworklabModule.cpp
    QT_SOURCES  platforms/qt/FrameworkLabView.cpp
                platforms/qt/FrameworkLabViewEntry.cpp
    IOS_SOURCES platforms/ios/FrameworkLabView.mm
                platforms/ios/FrameworkLabViewEntry.mm
)
