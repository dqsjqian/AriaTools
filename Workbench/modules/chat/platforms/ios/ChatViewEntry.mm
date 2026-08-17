#include "ChatView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/ChatVm.h"

namespace wb::chat {

void register_chat_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "chat", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::ChatView>(static_cast<ChatVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::chat
