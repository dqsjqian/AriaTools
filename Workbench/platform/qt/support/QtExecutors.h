#pragma once
//
// QtExecutors — adapters that wrap Aria's QtDispatcher as IExecutor /
// IDelayedScheduler, so ServiceHub can use the Qt main thread as the UI
// executor and delayed scheduler. Mirrors Aria demo1 Executors.h.
//
#include "aria/async/executor.hpp"
#include "aria/scheduler.hpp"
#include "aria/runtime/dispatcher.hpp"

#include <chrono>
#include <functional>
#include <utility>

namespace wb::qt {

/// IExecutor that forwards post() to a runtime::IDispatcher (Qt main thread).
/// Use case: UI executor for AsyncCommand.
class DispatcherExec final : public aria::async::IExecutor {
public:
    explicit DispatcherExec(aria::runtime::IDispatcher& d) noexcept : d_(d) {}
    void post(std::function<void()> fn) override { d_.post(std::move(fn)); }
private:
    aria::runtime::IDispatcher& d_;
};

/// IDelayedScheduler backed by IDispatcher::post_delayed.
/// Use case: reactive operators (debounce / throttle).
class DispatcherDelay final : public aria::IDelayedScheduler {
public:
    explicit DispatcherDelay(aria::runtime::IDispatcher& d) noexcept : d_(d) {}
    void post_after(std::chrono::milliseconds delay,
                    std::function<void()> fn) override {
        d_.post_delayed(delay, std::move(fn));
    }
private:
    aria::runtime::IDispatcher& d_;
};

}  // namespace wb::qt
