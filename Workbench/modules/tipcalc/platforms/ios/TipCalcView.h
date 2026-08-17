#pragma once
//
// TipCalcView — iOS UIKit view for the "tipcalc" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <vector>

namespace wb::tipcalc { class TipCalcVm; }

namespace wb::tipcalc::iosview {

class TipCalcView {
public:
    TipCalcView(TipCalcVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::tipcalc::iosview
