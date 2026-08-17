wb_add_module(NAME dashboard
    SOURCES     viewmodels/DashboardVm.cpp module/DashboardModule.cpp
    QT_SOURCES  platforms/qt/DashboardView.cpp
                platforms/qt/DashboardViewEntry.cpp
    IOS_SOURCES platforms/ios/DashboardView.mm platforms/ios/DashboardViewEntry.mm
)
