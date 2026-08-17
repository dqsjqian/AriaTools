#pragma once
//
// DashboardView — iOS UIKit view for the "dashboard" module.
//
// register_dashboard_view() is a thin entry point: it instantiates this class
// and returns its viewController().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::dashboard { class DashboardVm; }

namespace wb::dashboard::iosview {

class DashboardView {
public:
    DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::dashboard::iosview
