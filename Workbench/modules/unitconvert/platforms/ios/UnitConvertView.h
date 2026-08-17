#pragma once
//
// UnitConvertView — iOS UIKit view for the "unitconvert" module.
//
// The View receives the HostVm (UnitConvertVmHostVm) so it can bind
// host-owned title/desc properties; the inner UnitConvertVm is accessed
// via host.inner().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::unitconvert { class UnitConvertVmHostVm; }

namespace wb::unitconvert::iosview {

class UnitConvertView {
public:
    UnitConvertView(UnitConvertVmHostVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::unitconvert::iosview
