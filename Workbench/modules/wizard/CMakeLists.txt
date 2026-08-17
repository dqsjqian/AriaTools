wb_add_module(NAME wizard
    SOURCES viewmodels/WizardVm.cpp module/WizardModule.cpp
    QT_SOURCES  platforms/qt/WizardView.cpp
                platforms/qt/WizardViewEntry.cpp
    IOS_SOURCES platforms/ios/WizardView.mm platforms/ios/WizardViewEntry.mm
)
