#pragma once
//
// ModuleContext — Context injected into each module.
// Modules obtain stable services (ServiceHub) and the event bus through this;
// they never new services themselves and don't know concrete implementations.
//
#include "infra/ServiceHub.h"
#include "module_api/MountRegistry.h"
#include "module_api/NavigatorHost.h"
#include "aria/binding/view_model.hpp"
#include "aria/runtime/event_bus.hpp"
#include "aria/scheduler.hpp"

#include <functional>
#include <memory>
#include <string>

namespace wb::module_api {

/// Factory for creating a *fresh* ViewModel instance of another module.
/// Backed by AppCore::create_module_vm — each call builds a new instance so
/// navigation pages never share state with the main-tab module VM.
using ModuleVmFactory =
    std::function<std::shared_ptr<aria::binding::ViewModel>(const std::string&)>;

class ModuleContext {
public:
    explicit ModuleContext(wb::infra::ServiceHub& hub) : hub_(hub) {}

    [[nodiscard]] wb::infra::ServiceHub& services() { return hub_; }
    [[nodiscard]] aria::runtime::EventBus& bus() { return hub_.bus(); }

    /// UI-thread executor (platform-injected; InlineExecutor fallback).
    [[nodiscard]] aria::async::IExecutor& ui_exec() { return hub_.ui_exec(); }
    /// Shared worker thread pool.
    [[nodiscard]] aria::async::ThreadPoolExecutor& worker() { return hub_.worker(); }
    /// Delayed scheduler for debounce/throttle (platform-injected; immediate fallback).
    [[nodiscard]] aria::IDelayedScheduler& timer() { return hub_.timer(); }

    /// Fetch a stable service interface directly.
    template <typename I>
    [[nodiscard]] I& service() { return hub_.service<I>(); }

    /// Install the cross-module ViewModel factory (set by AppCore once).
    void set_vm_factory(ModuleVmFactory f) { vmFactory_ = std::move(f); }

    /// Create a fresh ViewModel instance of `moduleId` (cross-module
    /// navigation). Returns nullptr if the module id is unknown.
    [[nodiscard]] std::shared_ptr<aria::binding::ViewModel>
    create_module_vm(const std::string& moduleId) const {
        return vmFactory_ ? vmFactory_(moduleId) : nullptr;
    }

    /// Install the cross-module navigator (set by AppCore once). The
    /// navigator owns the module→page registry; VMs navigate via
    /// `ctx.navigator().Push<I>(params)`.
    void set_navigator(std::shared_ptr<NavigatorHost> nav) {
        navigator_ = std::move(nav);
    }

    /// Cross-module navigator (may be null before AppCore installs it).
    [[nodiscard]] NavigatorHost& navigator() {
        return *navigator_;
    }

    /// Install the cross-module mount registry (set by AppCore once). The
    /// registry owns the slot→provider map; hosts resolve slot content via
    /// `ctx.mounts().Resolve("dashboard.content")`.
    void set_mounts(std::shared_ptr<MountRegistry> mounts) {
        mounts_ = std::move(mounts);
    }

    /// Cross-module mount registry (may be null before AppCore installs it).
    [[nodiscard]] MountRegistry& mounts() {
        return *mounts_;
    }

private:
    wb::infra::ServiceHub& hub_;
    ModuleVmFactory vmFactory_;
    std::shared_ptr<NavigatorHost> navigator_;
    std::shared_ptr<MountRegistry> mounts_;
};

}  // namespace wb::module_api
