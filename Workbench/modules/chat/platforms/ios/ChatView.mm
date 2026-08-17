#include "ChatView.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/ChatVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::chat::iosview {

// ─── PublisherView ─────────────────────────────────────────────────────────
PublisherView::PublisherView(ChatVm& vm, aria::binding::BindingEngine& be)
    : view_([[UIStackView alloc] init]) {
    auto& pub = *vm.publisher;

    ((UIStackView*)view_).axis = UILayoutConstraintAxisHorizontal;
    ((UIStackView*)view_).spacing = 8;
    ((UIStackView*)view_).distribution = UIStackViewDistributionFill;

    UILabel*     userLbl   = wb::ios::ui::make_label(@"");
    UITextField* userField  = wb::ios::ui::make_field(@"");
    UILabel*     draftLbl  = wb::ios::ui::make_label(@"");
    UITextField* draftField = wb::ios::ui::make_field(@"");
    UIButton*    sendBtn   = wb::ios::ui::make_button(@"");

    [(UIStackView*)view_ addArrangedSubview:userLbl];
    [(UIStackView*)view_ addArrangedSubview:userField];
    [(UIStackView*)view_ addArrangedSubview:draftLbl];
    [(UIStackView*)view_ addArrangedSubview:draftField];
    [(UIStackView*)view_ addArrangedSubview:sendBtn];

    // Field labels + button (i18n).
    userLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("chat","user").c_str()];
    draftLbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("chat","draft").c_str()];
    [sendBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("chat","send").c_str()]
             forState:UIControlStateNormal];

    be.bind_text  (pub.user,  wb::ios::ui::view_for(userField));
    be.bind_text  (pub.draft, wb::ios::ui::view_for(draftField));
    be.bind_command(pub.send, wb::ios::ui::view_for(sendBtn));
}

// ─── SubscriberView ─────────────────────────────────────────────────────────
// The original iOS view did not bind subscriber-side data (no UIKit list
// adapter). Keep this as a labelled placeholder panel so the structure is
// symmetric with the Qt view without inventing new binding logic.
SubscriberView::SubscriberView(ChatVm& vm, aria::binding::BindingEngine& be)
    : view_(wb::ios::ui::make_label(@"")) {
    ((UILabel*)view_).text = @"Subscriber";
    (void)vm;
    (void)be;
}

// ─── Top-level ChatView ─────────────────────────────────────────────────────
ChatView::ChatView(ChatVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* desc  = wb::ios::ui::make_label(@"");

    PublisherView  pub(vm, be);
    SubscriberView sub(vm, be);

    vc_ = wb::ios::ui::make_stack_vc(@[title, desc, pub.view(), sub.view()]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc,  wb::ios::ui::view_for(desc));
}

}  // namespace wb::chat::iosview
