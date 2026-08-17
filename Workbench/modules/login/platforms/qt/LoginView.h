#pragma once
//
// LoginView — Qt view for the "login" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_login_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::login { class LoginVm; }

namespace wb::login::qtview {

QWidget* build_view(LoginVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::login::qtview
