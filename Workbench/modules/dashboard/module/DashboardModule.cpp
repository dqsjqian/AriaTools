#include "module/DashboardModule.h"
#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

std::shared_ptr<aria::binding::ViewModel>
DashboardModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    // Pass the context so the VM can resolve other modules' VMs for
    // cross-module navigation (VM-layer routing).
    return std::make_shared<DashboardVm>(ctx);
}

std::shared_ptr<wb::module_api::IModule> make_dashboard_module() {
    return std::make_shared<DashboardModule>();
}

}  // namespace wb::dashboard
