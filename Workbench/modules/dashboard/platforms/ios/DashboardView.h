#pragma once
//
// DashboardView — iOS UIKit view for the "dashboard" module.
//
// Cross-module navigation demo: buttons fire the VM's Commands (routing is
// decided in the VM layer); this view renders `nav->current` by embedding the
// target module's UIViewController as a child.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <memory>
#include <vector>

namespace wb::dashboard { class DashboardVm; }

namespace wb::dashboard::iosview {

class DashboardView {
public:
    DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }

private:
    UIViewController* __weak vc_;
    UIViewController* __weak pageHost_;         ///< embedded navigation target
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::dashboard::iosview
