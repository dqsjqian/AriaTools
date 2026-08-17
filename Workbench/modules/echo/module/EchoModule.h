#pragma once
#include "module_api/IModule.h"
namespace wb::echo {
class EchoModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "echo"; }
    std::string nav_key() const override { return "nav_echo"; }
    int order() const override { return 99; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};
std::shared_ptr<wb::module_api::IModule> make_echo_module();
}  // namespace wb::echo
