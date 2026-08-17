#import "CalendarViewController.h"
#include "support/UIViewFactory.h"
#include "viewmodels/CalendarVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar {

void register_calendar_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "calendar", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            // Controller owns the VM ref + does all bindings; View is pure UI.
            return (UIViewController*)[[CalendarViewController alloc]
                initWithViewModelRef:static_cast<CalendarVm&>(vm)
                              engine:be];
        });
}

}  // namespace wb::calendar
