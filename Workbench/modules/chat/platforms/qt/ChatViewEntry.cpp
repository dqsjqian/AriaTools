#include "ChatView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ChatVm.h"

namespace wb::chat {

void register_chat_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "chat",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::ChatView(static_cast<ChatVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::chat
