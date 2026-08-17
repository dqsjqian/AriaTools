#pragma once
//
// ThemeView — iOS UIKit view for the "theme" module.
//
// Binds directly to ThemeVm.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::theme { class ThemeVm; }

namespace wb::theme::iosview {

class ThemeView {
public:
    ThemeView(ThemeVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
};

}  // namespace wb::theme::iosview
