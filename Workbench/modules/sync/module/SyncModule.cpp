#include "module/SyncModule.h"
#include "viewmodels/SyncVm.h"

#include "infra/sync/ISyncService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/secret/ISecretStore.h"

namespace wb::sync {

std::shared_ptr<aria::binding::ViewModel>
SyncModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<SyncVm>(
        ctx.service<wb::services::ISyncService>(),
        ctx.service<wb::services::ISettingsService>(),
        ctx.service<wb::services::ISecretStore>());
}

std::shared_ptr<wb::module_api::IModule> make_sync_module() {
    return std::make_shared<SyncModule>();
}

}  // namespace wb::sync
