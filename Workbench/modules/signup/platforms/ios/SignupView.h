#pragma once
//
// SignupView — iOS UIKit view for the "signup" module.
//
// Binds directly to SignupVm.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::signup { class SignupVm; }

namespace wb::signup::iosview {

class SignupView {
public:
    SignupView(SignupVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
};

}  // namespace wb::signup::iosview
