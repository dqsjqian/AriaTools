#include "LoginView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/LoginVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::login::iosview {

LoginView::LoginView(LoginVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*     title    = wb::ios::ui::make_title(@"");
    UILabel*     desc     = wb::ios::ui::make_label(@"");
    UILabel*     userLbl  = wb::ios::ui::make_label(@"");
    UITextField* userField = wb::ios::ui::make_field(@"");
    UILabel*     pwdLbl   = wb::ios::ui::make_label(@"");
    UITextField* pwdField  = wb::ios::ui::make_field(@"");
    UIButton*    loginBtn = wb::ios::ui::make_button(@"");
    UILabel*     activeLbl = wb::ios::ui::make_label(@"");
    UILabel*     welcomeLbl = wb::ios::ui::make_label(@"");
    UILabel*     errLbl   = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, desc, userLbl, userField, pwdLbl, pwdField, loginBtn,
          activeLbl, welcomeLbl, errLbl]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc,  wb::ios::ui::view_for(desc));

    // Labels + button (i18n).
    userLbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("login","username").c_str()];
    pwdLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("login","password").c_str()];
    [loginBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("login","login").c_str()]
              forState:UIControlStateNormal];

    be.bind_text(vm.username, wb::ios::ui::view_for(userField));
    be.bind_text(vm.password, wb::ios::ui::view_for(pwdField));
    be.bind_command(vm.submitCmd, wb::ios::ui::view_for(loginBtn));

    // Active/inactive status label.
    be.bind_text_projected(vm.is_active(), wb::ios::ui::view_for(activeLbl),
        [](const bool a) {
            return wb::i18n::str_in("login", a ? "vm_active" : "vm_inactive");
        });
    // Welcome / not-logged-in.
    be.bind_optional_text(vm.login.last_result, wb::ios::ui::view_for(welcomeLbl),
        [](const LoginResult& r) { return r.welcome; },
        wb::i18n::str_in("login", "not_logged_in"));
    be.bind_text_oneway(vm.login.last_error_message, wb::ios::ui::view_for(errLbl));
}

}  // namespace wb::login::iosview

namespace wb::login {
void register_login_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "login", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::LoginView(static_cast<LoginVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::login
