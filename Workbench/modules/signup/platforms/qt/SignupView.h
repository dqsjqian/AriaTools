#pragma once
//
// SignupView — Qt view for the "signup" module.
//
// Binds directly to SignupVm.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::signup { class SignupVm; }

namespace wb::signup::qtview {

class SignupView {
public:
    SignupView(SignupVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::signup::qtview
