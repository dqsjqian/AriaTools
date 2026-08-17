wb_add_module(NAME wizard
    SOURCES viewmodels/WizardVm.cpp module/WizardModule.cpp
    QT_SOURCES  platforms/qt/WizardView.cpp
    IOS_SOURCES platforms/ios/WizardView.mm
)
