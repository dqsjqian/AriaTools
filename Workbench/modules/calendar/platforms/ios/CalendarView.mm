#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/CalendarVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar::iosview {
static UIViewController* build(CalendarVm& vm, aria::binding::BindingEngine& be) {
    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     month   = wb::ios::ui::make_label(@"");
    UIButton*    prev    = wb::ios::ui::make_button(@"");
    UIButton*    next    = wb::ios::ui::make_button(@"");
    UIButton*    today   = wb::ios::ui::make_button(@"");
    UIButton*    refresh = wb::ios::ui::make_button(@"");
    UITextField* url     = wb::ios::ui::make_field(@"");
    UIButton*    add     = wb::ios::ui::make_button(@"");
    UILabel*     status  = wb::ios::ui::make_label(@"");

    auto* vc = wb::ios::ui::make_stack_vc(
        @[title, month, prev, next, today, refresh, url, add, status]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.monthTitle, wb::ios::ui::view_for(month));
    be.bind_text_oneway(vm.prevLabel, wb::ios::ui::view_for(prev));
    be.bind_text_oneway(vm.nextLabel, wb::ios::ui::view_for(next));
    be.bind_text_oneway(vm.todayLabel, wb::ios::ui::view_for(today));
    be.bind_text_oneway(vm.refreshLabel, wb::ios::ui::view_for(refresh));
    be.bind_text_oneway(vm.subscribeLabel, wb::ios::ui::view_for(add));
    be.bind_text(vm.subscribeUrl, wb::ios::ui::view_for(url));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));

    be.bind_command(vm.prevMonth, wb::ios::ui::view_for(prev));
    be.bind_command(vm.nextMonth, wb::ios::ui::view_for(next));
    be.bind_command(vm.today, wb::ios::ui::view_for(today));
    be.bind_command(vm.refresh, wb::ios::ui::view_for(refresh));
    be.bind_command(vm.addSubscription, wb::ios::ui::view_for(add));
    return vc;
}
}  // namespace wb::calendar::iosview

namespace wb::calendar {
void register_calendar_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "calendar", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<CalendarVm&>(vm), be);
        });
}
}  // namespace wb::calendar
