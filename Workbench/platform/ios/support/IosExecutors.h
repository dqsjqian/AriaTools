#pragma once
//
// IosExecutors — adapters that dispatch tasks onto the iOS main queue
// (UIKit main thread), exposed as aria::async::IExecutor and
// aria::IDelayedScheduler so ServiceHub can use the UI thread as the UI
// executor and the delayed scheduler. This is iOS-specific: unlike Qt,
// Grand Central Dispatch does not implement Aria's IDispatcher contract.
//
#include "aria/async/executor.hpp"
#include "aria/scheduler.hpp"

#include <chrono>
#include <utility>

#import <dispatch/dispatch.h>

namespace wb::ios {

/// IExecutor that forwards post() to dispatch_get_main_queue() (UIKit main
/// thread). Use case: UI executor for AsyncCommand — the final co_await
/// schedule_on(ui) resumes here, keeping reactive Property writes on the
/// graph thread so the thread-affinity invariant is upheld.
class MainQueueExec final : public aria::async::IExecutor {
public:
    void post(std::function<void()> fn) override {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (fn) fn();
        });
    }
};

/// IDelayedScheduler backed by dispatch_after(). Use case: reactive
/// operators (debounce / throttle) that need to hop to the UI thread.
class MainQueueDelay final : public aria::IDelayedScheduler {
public:
    void post_after(std::chrono::milliseconds delay,
                    std::function<void()> fn) override {
        const auto when = dispatch_time(DISPATCH_TIME_NOW,
                                        static_cast<int64_t>(delay.count()) * NSEC_PER_MSEC);
        dispatch_after(when, dispatch_get_main_queue(), ^{
            if (fn) fn();
        });
    }
};

}  // namespace wb::ios