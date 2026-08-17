#include "DashboardView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/DashboardVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::dashboard::iosview {

DashboardView::DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* info  = wb::ios::ui::make_label(@"");
    vc_ = wb::ios::ui::make_stack_vc(@[title, info]);
    be.bind_text_oneway(vm.welcome, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ios::ui::view_for(info));
}

}  // namespace wb::dashboard::iosview

namespace wb::dashboard {
void register_dashboard_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "dashboard", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::DashboardView(static_cast<DashboardVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::dashboard
