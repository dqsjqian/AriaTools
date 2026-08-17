#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/ChatVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::chat::iosview {

static UIViewController* build(ChatVm& vm, aria::binding::BindingEngine& be) {
    UILabel*     title = wb::ios::ui::make_title(@"");
    UILabel*     desc  = wb::ios::ui::make_label(@"");
    auto& pub = *vm.publisher;

    UILabel*     userLbl   = wb::ios::ui::make_label(@"");
    UITextField* userField  = wb::ios::ui::make_field(@"");
    UILabel*     draftLbl  = wb::ios::ui::make_label(@"");
    UITextField* draftField = wb::ios::ui::make_field(@"");
    UIButton*    sendBtn   = wb::ios::ui::make_button(@"");

    auto* vc = wb::ios::ui::make_stack_vc(
        @[title, desc, userLbl, userField, draftLbl, draftField, sendBtn]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc,  wb::ios::ui::view_for(desc));

    // Field labels + button (i18n).
    userLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("chat","user").c_str()];
    draftLbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("chat","draft").c_str()];
    [sendBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("chat","send").c_str()]
             forState:UIControlStateNormal];

    be.bind_text  (pub.user,  wb::ios::ui::view_for(userField));
    be.bind_text  (pub.draft, wb::ios::ui::view_for(draftField));
    be.bind_command(pub.send, wb::ios::ui::view_for(sendBtn));
    return vc;
}

}  // namespace wb::chat::iosview

namespace wb::chat {
void register_chat_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "chat", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<ChatVm&>(vm), be);
        });
}
}  // namespace wb::chat
