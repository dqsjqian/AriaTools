wb_add_module(NAME calendar
    SOURCES     models/CalendarModel.cpp
                services/CalendarService.cpp
                viewmodels/CalendarVm.cpp
                module/CalendarModule.cpp
    QT_SOURCES  platforms/qt/CalendarView.cpp
    IOS_SOURCES platforms/ios/CalendarView.mm
)
