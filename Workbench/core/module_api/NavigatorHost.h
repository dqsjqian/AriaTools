#pragma once

// Workbench presents Aria's typed routes through the existing Navigator stack.
// JSON capability interfaces and Push/Modal/Window metadata remain app policy.
#include "module_api/INavigationTarget.h"
#include "module_api/NavigationEntryVm.h"
#include "aria/app/mount_registry.hpp"
#include "aria/binding/navigation.hpp"

namespace wb::module_api {
class ModuleContext;

class NavigatorHost {
public:
    explicit NavigatorHost(ModuleContext& context) : routes_(context) {}
    template<class Interface, class Factory>
    void Register(std::string moduleId, Factory factory) {
        static_assert(std::is_base_of_v<INavigationTarget, Interface>);
        registrations_ += routes_.template add<Interface>(std::move(moduleId), std::move(factory));
    }
    template<class Interface>
    bool Unregister() { return routes_.template remove<Interface>(); }

    template<class Interface, class Payload>
    bool Push(const Payload& payload, NavOptions options = {}) {
        auto result = routes_.template resolve<Interface>(payload);
        if (!result) return false;
        const auto kind = options.presentation == Presentation::Modal
            ? aria::binding::Presentation::Modal : aria::binding::Presentation::Push;
        stack_->push(std::make_shared<NavigationEntryVm>(
            std::move(result->provider_id), std::move(result->vm), options.presentation), kind);
        return true;
    }
    bool Pop() { return stack_->pop(); }
    [[nodiscard]] auto& current() { return stack_->current; }
    [[nodiscard]] auto& depth() { return stack_->depth; }
    void clear() {
        while (stack_->pop()) {}
        registrations_.clear();
        routes_.clear();
    }

private:
    aria::app::TypedRouteRegistry<ModuleContext> routes_;
    std::shared_ptr<aria::binding::Navigator> stack_{std::make_shared<aria::binding::Navigator>()};
    aria::SubscriptionBag registrations_;
};
}  // namespace wb::module_api
