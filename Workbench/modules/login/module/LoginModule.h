#pragma once
//
// LoginModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::login {

class LoginModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "login"; }
    std::string nav_key() const override { return "nav_login"; }
    int order() const override { return 11; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::login

namespace wb::module_api { class IModule; }
namespace wb::login {
std::shared_ptr<wb::module_api::IModule> make_login_module();
}
