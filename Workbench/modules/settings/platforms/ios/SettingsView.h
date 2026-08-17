#pragma once
//
// SettingsView — iOS UIKit view for the "settings" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::settings { class SettingsVm; }

namespace wb::settings::iosview {

class SettingsView {
public:
    SettingsView(SettingsVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::settings::iosview
