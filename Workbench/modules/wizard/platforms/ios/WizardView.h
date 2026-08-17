#pragma once
//
// WizardView — iOS UIKit view for the "wizard" module.
//
// Decomposed into sub-views (mirroring the Qt WizardView structure),
// one per step VM:
//   Step1View — username + email form (Step1Vm)
//   Step2View — theme picker buttons (Step2Vm)
//   Step3View — finish button + result label (Step3Vm)
//
// The top-level WizardView assembles them into a vertical stack VC and
// wires the host-owned title/desc.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::wizard {
class WizardVmHostVm;
class Step1Vm;
class Step2Vm;
class Step3Vm;
}

namespace wb::wizard::iosview {

// ─── Sub-views (each builds a UIView*) ────────────────────────────────────

class Step1View {
public:
    Step1View(Step1Vm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return view_; }
private:
    UIView* view_;
};

class Step2View {
public:
    Step2View(Step2Vm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return view_; }
private:
    UIView* view_;
};

class Step3View {
public:
    Step3View(Step3Vm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return view_; }
private:
    UIView* view_;
};

// ─── Top-level WizardView ─────────────────────────────────────────────────

class WizardView {
public:
    WizardView(WizardVmHostVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::wizard::iosview
