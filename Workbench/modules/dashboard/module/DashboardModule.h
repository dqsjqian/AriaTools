#pragma once
//
// DashboardModule — IModule implementation + factory declaration for the dashboard module.
//
#include "module_api/IModule.h"

namespace wb::dashboard {

class DashboardModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "dashboard"; }
    std::string nav_key() const override { return "nav_dashboard"; }
    int order() const override { return 0; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::dashboard

// Factory: explicitly invoked by the app layer's ModulesManifest (avoids static library global registration being stripped).
namespace wb::module_api { class IModule; }
namespace wb::dashboard {
std::shared_ptr<wb::module_api::IModule> make_dashboard_module();
}
