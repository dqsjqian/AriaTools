#pragma once
//
// UnitConvertView — iOS UIKit view for the "unitconvert" module.
//
// Binds directly to UnitConvertVm.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <vector>

namespace wb::unitconvert { class UnitConvertVm; }

namespace wb::unitconvert::iosview {

class UnitConvertView {
public:
    UnitConvertView(UnitConvertVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::unitconvert::iosview
