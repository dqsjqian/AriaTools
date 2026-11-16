#include "ChatView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ChatVm.h"

namespace wb::chat {

void register_chat_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "chat",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::ChatView>(static_cast<ChatVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::chat
