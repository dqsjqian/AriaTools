#include "DashboardView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

void register_dashboard_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "dashboard", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::DashboardView>(static_cast<DashboardVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::dashboard
