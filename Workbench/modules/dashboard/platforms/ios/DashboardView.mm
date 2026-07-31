#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/DashboardVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::dashboard::iosview {
static UIViewController* build(DashboardVm& vm, aria::binding::BindingEngine& be) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* info  = wb::ios::ui::make_label(@"");
    auto* vc = wb::ios::ui::make_stack_vc(@[title, info]);
    be.bind_text_oneway(vm.welcome, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ios::ui::view_for(info));
    return vc;
}
}  // namespace wb::dashboard::iosview

namespace wb::dashboard {
void register_dashboard_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "dashboard", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<DashboardVm&>(vm), be);
        });
}
}  // namespace wb::dashboard
