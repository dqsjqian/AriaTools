#pragma once
//
// CartView — iOS UIKit view for the "cart" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <vector>

namespace wb::cart { class CartVm; }

namespace wb::cart::iosview {

class CartView {
public:
    CartView(CartVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::cart::iosview
