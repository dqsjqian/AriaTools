#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/CalendarVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar::iosview {
static UIViewController* build(CalendarVm& vm, aria::binding::BindingEngine& be) {
    UILabel*     title  = wb::ios::ui::make_title(@"");
    UITextField* url    = wb::ios::ui::make_field(@"");
    UIButton*    add    = wb::ios::ui::make_button(@"");
    UILabel*     status = wb::ios::ui::make_label(@"");
    auto* vc = wb::ios::ui::make_stack_vc(@[title, url, add, status]);
    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.subscribeLabel, wb::ios::ui::view_for(add));
    be.bind_text(vm.subscribeUrl, wb::ios::ui::view_for(url));
    be.bind_command(vm.addSubscription, wb::ios::ui::view_for(add));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
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
