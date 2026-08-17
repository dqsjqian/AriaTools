#pragma once
//
// SignupView — iOS UIKit view for the "signup" module.
//
// The View receives the HostVm (SignupVmHostVm) so it can bind
// host-owned title/desc properties; the inner SignupVm is accessed
// via host.inner().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::signup { class SignupVmHostVm; }

namespace wb::signup::iosview {

class SignupView {
public:
    SignupView(SignupVmHostVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::signup::iosview
