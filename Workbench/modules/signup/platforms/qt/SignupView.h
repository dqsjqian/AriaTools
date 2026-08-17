#pragma once
//
// SignupView — Qt view for the "signup" module (Aria free-function view).
//
// Uses the HostVm pattern: build_view() receives SignupVmHostVm (so it can
// read host.desc) and derives the inner SignupVm from it.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::signup { class SignupVmHostVm; }

namespace wb::signup::qtview {

QWidget* build_view(SignupVmHostVm& host, aria::binding::BindingEngine& be);

}  // namespace wb::signup::qtview
