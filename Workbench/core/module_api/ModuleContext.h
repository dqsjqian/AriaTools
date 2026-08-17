#pragma once
//
// ModuleContext — Context injected into each module.
// Modules obtain stable services (ServiceHub) and the event bus through this;
// they never new services themselves and don't know concrete implementations.
//
#include "infra/ServiceHub.h"
#include "aria/runtime/event_bus.hpp"
#include "aria/scheduler.hpp"

namespace wb::module_api {

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

private:
    wb::infra::ServiceHub& hub_;
};

}  // namespace wb::module_api
