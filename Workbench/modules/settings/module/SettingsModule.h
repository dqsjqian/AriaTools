#pragma once
#include "module_api/IModule.h"

namespace wb::settings {

class SettingsModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "settings"; }
    std::string nav_key() const override { return "nav_settings"; }
    int order() const override { return 4; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

std::shared_ptr<wb::module_api::IModule> make_settings_module();

}  // namespace wb::settings
