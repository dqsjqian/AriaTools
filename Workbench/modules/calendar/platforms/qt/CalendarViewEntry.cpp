#include "CalendarView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

void register_calendar_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "calendar",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::CalendarView>(static_cast<CalendarVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::calendar
