#pragma once
//
// WizardView — Qt view for the "wizard" module (Aria free-function view).
//
// Decomposed into sub-views, one per step VM:
//   step 1 account  — username + email form (Step1Vm)
//   step 2 theme    — theme picker (Step2Vm)
//   step 3 confirm  — summary + finish button (Step3Vm)
//
// The top-level build_view() assembles them and wires navigation.
// register_wizard_view() registers the builder with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::wizard { class WizardVmHostVm; }

namespace wb::wizard::qtview {

QWidget* build_view(WizardVmHostVm& host, aria::binding::BindingEngine& be);

}  // namespace wb::wizard::qtview
