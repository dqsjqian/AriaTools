#include "module/SignupModule.h"
#include "viewmodels/SignupVm.h"
#include "viewmodels/SignupVmHostVm.h"

namespace wb::signup {

std::shared_ptr<aria::binding::ViewModel>
SignupModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        (void)ctx; return std::make_shared<SignupVmHostVm>();
}

std::shared_ptr<wb::module_api::IModule> make_signup_module() {
    return std::make_shared<SignupModule>();
}

}  // namespace wb::signup
