#pragma once
//
// IosShell — iOS-side shell. Holds UIKitAdapter + BindingEngine, reuses wb::core::AppCore.
// Views are built by UIViewFactory keyed by moduleId (symmetric with Qt).
//
#import <UIKit/UIKit.h>

#include "app/AppCore.h"
#include "aria/adapters/uikit/UIKitAdapter.hpp"
#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include "support/IosExecutors.h"

#include <memory>

namespace wb::ios {

class IosShell {
public:
    IosShell();
    ~IosShell();

    IosShell(const IosShell&) = delete;
    IosShell& operator=(const IosShell&) = delete;

    UIViewController* build_root();

private:
    std::shared_ptr<aria::adapters::uikit::UIKitAdapter> adapter_;
    aria::binding::BindingEngine                         be_;
    MainQueueExec                                        ui_exec_;
    MainQueueDelay                                       delay_;
    wb::core::AppCore                                    core_;
    aria::Subscription                                   lang_sub_;
};

}  // namespace wb::ios
