#include "module/ToolsModule.h"

#include "models/ToolsModel.h"
#include "services/ToolsService.h"
#include "viewmodels/ToolsVm.h"

namespace wb::tools {

void ToolsModule::ensure_initialized_(wb::module_api::ModuleContext& ctx) {
    if (model_) return;
    service_ = std::make_shared<ToolsService>(
        ctx.services().crypto(), ctx.services().storage());
    model_ = std::make_shared<ToolsModel>(service_);
}

std::shared_ptr<aria::binding::ViewModel>
ToolsModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    ensure_initialized_(ctx);
    return std::make_shared<ToolsVm>(ctx.services().i18n(), model_);
}

std::shared_ptr<wb::module_api::IModule> make_tools_module() {
    return std::make_shared<ToolsModule>();
}

}  // namespace wb::tools
