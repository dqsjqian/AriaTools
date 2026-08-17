#pragma once
//
// ThemeView — iOS UIKit view for the "theme" module.
//
// The View receives the HostVm (ThemeVmHostVm) so it can bind
// host-owned title/desc properties; the inner ThemeVm is accessed
// via host.inner().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::theme { class ThemeVmHostVm; }

namespace wb::theme::iosview {

class ThemeView {
public:
    ThemeView(ThemeVmHostVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::theme::iosview
