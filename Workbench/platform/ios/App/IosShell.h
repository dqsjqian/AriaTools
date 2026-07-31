#pragma once
//
// IosShell — iOS 端外壳。持有 UIKitAdapter + BindingEngine，复用 wb::core::AppCore。
// View 通过 UIViewFactory 按 moduleId 构建（与 Qt 对称）。
//
#import <UIKit/UIKit.h>

#include "app/AppCore.h"
#include "aria/adapters/uikit/UIKitAdapter.hpp"
#include "aria/binding/binding_engine.hpp"

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
    wb::core::AppCore                                    core_;
};

}  // namespace wb::ios
