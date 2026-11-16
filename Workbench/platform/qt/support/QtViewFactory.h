#pragma once
//
// QtViewFactory — Qt-side "moduleId -> build View" registry.
// Each module's platforms/qt sources call register() to register its build function;
// the platform shell looks them up by moduleId. Module Views static_cast the base VM to the concrete type here as needed.
//
#include "aria/binding/view_model.hpp"
#include "aria/app/extension_registry.hpp"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace wb::qt {

using ViewBuilder =
    std::function<QWidget*(aria::binding::ViewModel&, aria::binding::BindingEngine&)>;

// View wrappers can own subscriptions and callbacks independently of QWidget.
// Release them at QObject::destroyed, before the root's child controls disappear.
template<class View>
QWidget* own_view(std::shared_ptr<View> view) {
    auto* root = view->widget();
    QObject::connect(root, &QObject::destroyed,
        [view = std::move(view)]() mutable { view.reset(); });
    return root;
}

class QtViewFactory {
public:
    static QtViewFactory& instance();

    void register_builder(const std::string& moduleId, ViewBuilder b) {
        registrations_.erase(moduleId);
        registrations_.emplace(moduleId, builders_.add(moduleId, std::move(b)));
    }

    [[nodiscard]] bool has_builder(const std::string& moduleId) const {
        return builders_.contains(moduleId);
    }

    [[nodiscard]] QWidget* build(const std::string& moduleId,
                                 aria::binding::ViewModel& vm,
                                 aria::binding::BindingEngine& be) const {
        auto builder = builders_.find(moduleId);
        if (!builder) return new QWidget;
        return (*builder)(vm, be);
    }

private:
    aria::app::ExtensionRegistry<std::string, ViewBuilder> builders_;
    std::unordered_map<std::string, aria::Subscription> registrations_;
};

}  // namespace wb::qt
