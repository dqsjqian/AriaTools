#include "CalendarView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

void register_calendar_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "calendar",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::CalendarView(static_cast<CalendarVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::calendar
