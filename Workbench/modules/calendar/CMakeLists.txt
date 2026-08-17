wb_add_module(NAME calendar
    SOURCES     models/CalendarModel.cpp
                services/CalendarService.cpp
                viewmodels/CalendarVm.cpp
                module/CalendarModule.cpp
    QT_SOURCES  platforms/qt/CalendarView.cpp
                platforms/qt/CalendarViewEntry.cpp
    IOS_SOURCES platforms/ios/CalendarView.mm
                platforms/ios/CalendarViewController.mm
                platforms/ios/CalendarViewEntry.mm
)
