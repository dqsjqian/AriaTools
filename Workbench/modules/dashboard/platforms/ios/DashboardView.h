#pragma once
//
// DashboardView — iOS UIKit view for the "dashboard" module.
//
// Cross-module navigation demo: buttons fire the VM's Commands (routing is
// decided in the VM layer); this view renders `nav->current` by the entry's
// Presentation kind:
//   Push   -> embed the target module's UIViewController as a child
//   Modal  -> present it (wrapped in a UINavigationController with a Done
//             button); swipe-down dismiss also pops the stack entry
//   Window -> mobile has no separate-window concept: falls back to modal
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
    UIViewController* __weak pageHost_;   ///< embedded navigation target
    NSMutableArray* presentedSentinels_;  ///< keepalive for modal dismiss delegates
    std::vector<aria::Subscription> subscriptions_;
};

}  // namespace wb::dashboard::iosview
