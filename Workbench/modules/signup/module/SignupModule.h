#pragma once
//
// SignupModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::signup {

class SignupModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "signup"; }
    std::string nav_key() const override { return "nav_signup"; }
    int order() const override { return 9; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::signup

namespace wb::module_api { class IModule; }
namespace wb::signup {
std::shared_ptr<wb::module_api::IModule> make_signup_module();
}
