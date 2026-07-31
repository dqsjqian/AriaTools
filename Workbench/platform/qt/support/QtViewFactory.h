#pragma once
//
// QtViewFactory — Qt 端「moduleId → 构建 View」登记表。
// 每个模块的 platforms/qt 源里调用 register()，把自己的 build 函数登记进来；
// 平台外壳按 moduleId 取用。模块 View 在此按需 static_cast 基类 VM 到具体类型。
//
#include "aria/binding/view_model.hpp"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace wb::qt {

using ViewBuilder =
    std::function<QWidget*(aria::binding::ViewModel&, aria::binding::BindingEngine&)>;

class QtViewFactory {
public:
    static QtViewFactory& instance();

    void register_builder(const std::string& moduleId, ViewBuilder b) {
        builders_[moduleId] = std::move(b);
    }

    [[nodiscard]] QWidget* build(const std::string& moduleId,
                                 aria::binding::ViewModel& vm,
                                 aria::binding::BindingEngine& be) const {
        auto it = builders_.find(moduleId);
        if (it == builders_.end()) return new QWidget;
        return it->second(vm, be);
    }

private:
    std::unordered_map<std::string, ViewBuilder> builders_;
};

}  // namespace wb::qt
