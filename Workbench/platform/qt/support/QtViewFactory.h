#pragma once
//
// QtViewFactory — Qt-side "moduleId -> build View" registry.
// Each module's platforms/qt sources call register() to register its build function;
// the platform shell looks them up by moduleId. Module Views static_cast the base VM to the concrete type here as needed.
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
