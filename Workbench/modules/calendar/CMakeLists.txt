wb_add_module(NAME calendar
    SOURCES     viewmodels/CalendarVm.cpp module/CalendarModule.cpp
    QT_SOURCES  platforms/qt/CalendarView.cpp
    IOS_SOURCES platforms/ios/CalendarView.mm
)
