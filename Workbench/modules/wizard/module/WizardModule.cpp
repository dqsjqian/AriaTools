#include "module/WizardModule.h"
#include "viewmodels/WizardVm.h"
#include "viewmodels/WizardVmHostVm.h"

namespace wb::wizard {

std::shared_ptr<aria::binding::ViewModel>
WizardModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        (void)ctx; return std::make_shared<WizardVmHostVm>();
}

std::shared_ptr<wb::module_api::IModule> make_wizard_module() {
    return std::make_shared<WizardModule>();
}

}  // namespace wb::wizard
