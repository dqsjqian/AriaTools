#include "module/LoginModule.h"
#include "viewmodels/LoginVm.h"

namespace wb::login {

std::shared_ptr<aria::binding::ViewModel>
LoginModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        return std::make_shared<LoginVm>(ctx.ui_exec(), ctx.worker());
}

std::shared_ptr<wb::module_api::IModule> make_login_module() {
    return std::make_shared<LoginModule>();
}

}  // namespace wb::login
