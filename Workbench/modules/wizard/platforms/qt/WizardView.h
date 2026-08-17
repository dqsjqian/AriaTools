#pragma once
//
// WizardView — Qt view for the "wizard" module.
//
// Decomposed into sub-views, one per step VM:
//   Step1AccountView — username + email form (Step1Vm)
//   Step2ThemeView   — theme picker (Step2Vm)
//   Step3ConfirmView — summary + finish button (Step3Vm)
//
// The top-level WizardView assembles them and wires navigation.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

#include <memory>

namespace wb::wizard {
class WizardVm;
class WizardVmHostVm;
class Step1Vm;
class Step2Vm;
class Step3Vm;
}

namespace wb::wizard::qtview {

class Step1AccountView {
public:
    Step1AccountView(Step1Vm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return w_; }
private:
    QWidget* w_;
};

class Step2ThemeView {
public:
    Step2ThemeView(Step2Vm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return w_; }
private:
    QWidget* w_;
};

class Step3ConfirmView {
public:
    Step3ConfirmView(Step3Vm& vm, aria::binding::BindingEngine& be,
                     std::vector<aria::Subscription>& subs);
    QWidget* widget() const { return w_; }
private:
    QWidget* w_;
};

class WizardView {
public:
    WizardView(WizardVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::wizard::qtview
