#pragma once
//
// SignupView — Qt view for the "signup" module.
//
// Uses the HostVm pattern: the constructor receives SignupVmHostVm (so it
// can read host.desc) and derives the inner SignupVm from it.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::signup { class SignupVmHostVm; }

namespace wb::signup::qtview {

class SignupView {
public:
    SignupView(SignupVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::signup::qtview
