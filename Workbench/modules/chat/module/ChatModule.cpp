#include "module/ChatModule.h"
#include "viewmodels/ChatVm.h"

namespace wb::chat {

std::shared_ptr<aria::binding::ViewModel>
ChatModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        return std::make_shared<ChatVm>(ctx.bus());
}

std::shared_ptr<wb::module_api::IModule> make_chat_module() {
    return std::make_shared<ChatModule>();
}

}  // namespace wb::chat
