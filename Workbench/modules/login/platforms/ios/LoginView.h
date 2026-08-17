#pragma once
//
// LoginView — iOS UIKit view for the "login" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::login { class LoginVm; }

namespace wb::login::iosview {

class LoginView {
public:
    LoginView(LoginVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::login::iosview
