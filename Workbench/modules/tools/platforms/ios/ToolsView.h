#pragma once
//
// ToolsView — iOS UIKit view for the "tools" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::tools { class ToolsVm; }

namespace wb::tools::iosview {

class ToolsView {
public:
    ToolsView(ToolsVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
};

}  // namespace wb::tools::iosview
