#import "CalendarViewController.h"
#include "support/UIViewFactory.h"
#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

void register_calendar_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "calendar", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return (UIViewController*)[[CalendarViewController alloc]
                initWithViewModelRef:static_cast<CalendarVm&>(vm)
                              engine:be];
        });
}

}  // namespace wb::calendar
