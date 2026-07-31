#include "module/SettingsModule.h"
#include "viewmodels/SettingsVm.h"

#include "infra/settings/ISettingsService.h"
#include "infra/secret/ISecretStore.h"

namespace wb::settings {

std::shared_ptr<aria::binding::ViewModel>
SettingsModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<SettingsVm>(
        ctx.services().i18n(),
        ctx.service<wb::services::ISettingsService>(),
        ctx.service<wb::services::ISecretStore>());
}

std::shared_ptr<wb::module_api::IModule> make_settings_module() {
    return std::make_shared<SettingsModule>();
}

}  // namespace wb::settings
