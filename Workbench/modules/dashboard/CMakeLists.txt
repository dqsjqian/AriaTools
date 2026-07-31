wb_add_module(NAME dashboard
    SOURCES     viewmodels/DashboardVm.cpp module/DashboardModule.cpp
    QT_SOURCES  platforms/qt/DashboardView.cpp
    IOS_SOURCES platforms/ios/DashboardView.mm
)
