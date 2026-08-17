#pragma once

#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::echo { class EchoVm; }

namespace wb::echo::iosview {

class EchoView {
public:
    EchoView(EchoVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }

private:
    UIViewController* __weak vc_;
};

}  // namespace wb::echo::iosview
