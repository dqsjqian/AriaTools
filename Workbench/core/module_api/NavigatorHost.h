#pragma once
//
// NavigatorHost — cross-module navigation registry.
//
// Modules self-register "which interface → which module page" via
// Register<I>(moduleId, factory). A caller navigates with Push<I> — fully
// typed, no string scheme keys at the call site, and the caller only needs
// the interface header from module_api.
//
// Decoupling contract:
//   * The interface I lives in module_api (framework kernel).
//   * The business VM implements I and registers a factory for it.
//   * If no module implements I (module removed / never installed),
//     Push<I> returns false and the caller degrades gracefully.
//
// Two independent payload channels (the target receives the exact type the
// caller pushed — C++ overload resolution dispatches, no forced json):
//   * typed struct:  Push<I>(CartArgs{...}) → I::on_navigate(const CartArgs&)
//   * raw json:      Push<I>(json{...})     → I::on_navigate(const json&)
//
// The host owns an Aria `Navigator` stack; the View layer renders
// `current()` the same way it rendered nav->current before.
//

#include "module_api/INavigationTarget.h"
#include "module_api/NavigationEntryVm.h"

#include "aria/binding/navigation.hpp"
#include "aria/binding/view_model.hpp"

#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace wb::module_api {

class ModuleContext;

class NavigatorHost {
public:
    explicit NavigatorHost(ModuleContext& ctx) : ctx_(ctx) {}

    /// Module self-registration: map interface I (implemented by the
    /// module's VM) to a factory that creates a fresh VM instance.
    /// The factory receives ModuleContext so it can inject services
    /// without the module capturing anything at registration time.
    template<typename I, typename Fn>
    void Register(std::string moduleId, Fn factory) {
        static_assert(std::is_base_of_v<INavigationTarget, I>,
                      "Navigation target interfaces must derive from "
                      "INavigationTarget");
        factories_[typeid(I)] =
            Entry{std::move(moduleId), std::move(factory)};
    }

    /// Navigate with either a typed struct payload (e.g. CartArgs) or a raw
    /// json object. Overload resolution on the target interface decides
    /// which on_navigate overload receives the payload; the target's return
    /// value tells us whether it consumed it. Returns false when the
    /// interface is unregistered OR the target does not handle this payload
    /// (so the caller knows up front whether delivery succeeded).
    template<typename I, typename Payload>
    bool Push(const Payload& payload) {
        auto it = factories_.find(typeid(I));
        if (it == factories_.end()) return false;   // no implementation → return

        auto vm = it->second.factory(ctx_);
        if (!vm) return false;

        // Deliver the payload to the exact on_navigate overload the caller's
        // type selects — typed struct goes to on_navigate(const Args&), raw
        // json to on_navigate(const json&). The target reports whether it
        // consumed the payload; false → do not push, report failure.
        if (auto* target = dynamic_cast<I*>(vm.get())) {
            if (!target->on_navigate(payload)) {
                return false;   // target does not handle this payload
            }
        }

        stack_->push(std::make_shared<NavigationEntryVm>(
            it->second.moduleId, std::move(vm)));
        return true;
    }

    /// Pop the navigation stack. Returns false if the stack is empty.
    bool Pop() { return stack_->pop(); }

    /// The topmost entry's ViewModel (nullptr at root) — View layer binds
    /// to this to render the current page.
    [[nodiscard]] aria::Property<std::shared_ptr<aria::binding::ViewModel>>&
    current() { return stack_->current; }
    [[nodiscard]] aria::Property<std::size_t>& depth() { return stack_->depth; }

private:
    struct Entry {
        std::string moduleId;
        std::function<std::shared_ptr<aria::binding::ViewModel>(ModuleContext&)>
            factory;
    };

    ModuleContext& ctx_;
    std::shared_ptr<aria::binding::Navigator> stack_{
        std::make_shared<aria::binding::Navigator>()};
    std::unordered_map<std::type_index, Entry> factories_;
};

}  // namespace wb::module_api
