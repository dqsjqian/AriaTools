#pragma once
//
// SyncView — iOS UIKit view for the "sync" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::sync { class SyncVm; }

namespace wb::sync::iosview {

class SyncView {
public:
    SyncView(SyncVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::sync::iosview
