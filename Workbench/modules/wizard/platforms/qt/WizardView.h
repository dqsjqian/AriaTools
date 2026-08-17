#pragma once
//
// WizardView — Qt view for the "wizard" module.
//
// Decomposed into sub-views, one per step VM:
//   step 1 account  — username + email form (Step1Vm)
//   step 2 theme    — theme picker (Step2Vm)
//   step 3 confirm  — summary + finish button (Step3Vm)
//
// The top-level WizardView constructor assembles them and wires navigation.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::wizard { class WizardVmHostVm; }

namespace wb::wizard::qtview {

class WizardView {
public:
    WizardView(WizardVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::wizard::qtview
