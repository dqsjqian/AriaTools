#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/SignupVm.h"
#include "viewmodels/SignupVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::signup::iosview {

static UIViewController* build(SignupVm& vm, SignupVmHostVm& host,
                                aria::binding::BindingEngine& be) {
    UILabel*     title    = wb::ios::ui::make_title(@"");
    UILabel*     desc     = wb::ios::ui::make_label(@"");
    UILabel*     userLbl  = wb::ios::ui::make_label(@"");
    UITextField* userField = wb::ios::ui::make_field(@"");
    UILabel*     userHint  = wb::ios::ui::make_label(@"");
    UILabel*     emailLbl  = wb::ios::ui::make_label(@"");
    UITextField* emailField = wb::ios::ui::make_field(@"");
    UILabel*     emailHint  = wb::ios::ui::make_label(@"");
    UILabel*     pwdLbl   = wb::ios::ui::make_label(@"");
    UITextField* pwdField  = wb::ios::ui::make_field(@"");
    UILabel*     pwdHint   = wb::ios::ui::make_label(@"");
    UILabel*     confLbl  = wb::ios::ui::make_label(@"");
    UITextField* confField = wb::ios::ui::make_field(@"");
    UILabel*     confHint  = wb::ios::ui::make_label(@"");
    UIButton*    submitBtn = wb::ios::ui::make_button(@"");
    UILabel*     formError = wb::ios::ui::make_label(@"");
    UILabel*     summary   = wb::ios::ui::make_label(@"");

    auto* vc = wb::ios::ui::make_stack_vc(
        @[title, desc,
          userLbl, userField, userHint,
          emailLbl, emailField, emailHint,
          pwdLbl, pwdField, pwdHint,
          confLbl, confField, confHint,
          submitBtn, formError, summary]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));

    // Field labels (i18n).
    userLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("signup","username").c_str()];
    emailLbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("signup","email").c_str()];
    pwdLbl.text   = [NSString stringWithUTF8String:wb::i18n::str_in("signup","password").c_str()];
    confLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("signup","confirm").c_str()];
    [submitBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("signup","submit").c_str()]
               forState:UIControlStateNormal];

    be.bind_text(vm.username.value, wb::ios::ui::view_for(userField));
    be.bind_text(vm.email.value,    wb::ios::ui::view_for(emailField));
    be.bind_text(vm.password.value, wb::ios::ui::view_for(pwdField));
    be.bind_text(vm.confirm.value,  wb::ios::ui::view_for(confField));

    // Per-field error hints.
    be.bind_text_oneway(vm.username.error, wb::ios::ui::view_for(userHint));
    be.bind_text_oneway(vm.email.error,    wb::ios::ui::view_for(emailHint));
    be.bind_text_oneway(vm.password.error, wb::ios::ui::view_for(pwdHint));
    be.bind_text_oneway(vm.confirm.error,  wb::ios::ui::view_for(confHint));

    // Form-level first error + submit result.
    be.bind_text_oneway(vm.form.first_error,  wb::ios::ui::view_for(formError));
    be.bind_text_oneway(vm.submittedSummary,  wb::ios::ui::view_for(summary));
    be.bind_command(vm.submit, wb::ios::ui::view_for(submitBtn));
    return vc;
}

}  // namespace wb::signup::iosview

namespace wb::signup {
void register_signup_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "signup", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<SignupVmHostVm&>(vm);
            return iosview::build(host.inner(), host, be);
        });
}
}  // namespace wb::signup
