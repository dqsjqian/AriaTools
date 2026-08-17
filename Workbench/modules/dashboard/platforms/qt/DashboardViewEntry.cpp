#include "DashboardView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

void register_dashboard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "dashboard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::DashboardView(static_cast<DashboardVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::dashboard
