#include "module/DashboardModule.h"
#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

std::shared_ptr<aria::binding::ViewModel>
DashboardModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<DashboardVm>();
}

std::shared_ptr<wb::module_api::IModule> make_dashboard_module() {
    return std::make_shared<DashboardModule>();
}

}  // namespace wb::dashboard
