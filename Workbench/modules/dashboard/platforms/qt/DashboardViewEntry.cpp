#include "DashboardView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

void register_dashboard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "dashboard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::DashboardView>(static_cast<DashboardVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::dashboard
