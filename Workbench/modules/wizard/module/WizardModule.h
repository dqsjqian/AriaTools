#pragma once
//
// WizardModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::wizard {

class WizardModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "wizard"; }
    std::string nav_key() const override { return "nav_wizard"; }
    int order() const override { return 14; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::wizard

namespace wb::module_api { class IModule; }
namespace wb::wizard {
std::shared_ptr<wb::module_api::IModule> make_wizard_module();
}
