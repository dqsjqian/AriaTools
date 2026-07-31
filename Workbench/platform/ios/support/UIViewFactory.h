#pragma once
//
// UIViewFactory — iOS 端「moduleId → 构建 UIViewController」登记表。
// 与 Qt 的 QtViewFactory 对称。各模块 platforms/ios 源调用 register()。
//
#import <UIKit/UIKit.h>

#include "aria/binding/view_model.hpp"
#include "aria/binding/binding_engine.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace wb::ios {

using ViewBuilder =
    std::function<UIViewController*(aria::binding::ViewModel&, aria::binding::BindingEngine&)>;

class UIViewFactory {
public:
    static UIViewFactory& instance();

    void register_builder(const std::string& moduleId, ViewBuilder b) {
        builders_[moduleId] = std::move(b);
    }

    [[nodiscard]] UIViewController* build(const std::string& moduleId,
                                          aria::binding::ViewModel& vm,
                                          aria::binding::BindingEngine& be) const {
        auto it = builders_.find(moduleId);
        if (it == builders_.end()) return [[UIViewController alloc] init];
        return it->second(vm, be);
    }

private:
    std::unordered_map<std::string, ViewBuilder> builders_;
};

}  // namespace wb::ios
