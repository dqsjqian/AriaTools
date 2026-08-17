#pragma once
//
// LoginView — Qt view for the "login" module.
//
// Owns the login form, the spinner/error/welcome/active status labels,
// and the hand-wired button enabled/text composition. register_login_view()
// is a thin entry point that constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::login { class LoginVm; }

namespace wb::login::qtview {

class LoginView {
public:
    LoginView(LoginVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::login::qtview
