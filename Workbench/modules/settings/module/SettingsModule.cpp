#include "module/SettingsModule.h"
#include "viewmodels/SettingsVm.h"

namespace wb::settings {

std::shared_ptr<aria::binding::ViewModel>
SettingsModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<SettingsVm>(ctx.i18n_context());
}

std::shared_ptr<wb::module_api::IModule> make_settings_module() {
    return std::make_shared<SettingsModule>();
}

}  // namespace wb::settings
