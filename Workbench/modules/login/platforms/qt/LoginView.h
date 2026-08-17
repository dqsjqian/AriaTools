#pragma once
//
// LoginView — Qt view for the "login" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
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
