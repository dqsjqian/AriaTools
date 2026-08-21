#pragma once

#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <vector>

namespace wb::frameworklab { class FrameworkLabVm; }

namespace wb::frameworklab::iosview {

class FrameworkLabView {
public:
    FrameworkLabView(FrameworkLabVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }

private:
    UIViewController* __weak vc_;
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::frameworklab::iosview
