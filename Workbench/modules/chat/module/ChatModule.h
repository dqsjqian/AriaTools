#pragma once
//
// ChatModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::chat {

class ChatModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "chat"; }
    std::string nav_key() const override { return "nav_chat"; }
    int order() const override { return 12; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::chat

namespace wb::module_api { class IModule; }
namespace wb::chat {
std::shared_ptr<wb::module_api::IModule> make_chat_module();
}
