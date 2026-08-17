#pragma once
//
// CalendarView — iOS UIKit view for the "calendar" module.
//
// Decomposed into sub-views (mirroring the Qt CalendarView structure):
//   MonthNavView       — prev/next/today/refresh buttons + month title
//   SubscriptionBarView — URL field + subscribe button
//   SubscriptionListView — list of subscriptions (double-tap to remove)
//
// The top-level CalendarView assembles them into a vertical stack VC and
// wires the shared VM. register_calendar_view() is a thin entry point.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar { class CalendarVm; }

namespace wb::calendar::iosview {

// ─── Sub-views (each builds a UIView*) ────────────────────────────────────

class MonthNavView {
public:
    MonthNavView(CalendarVm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return nav_; }
private:
    UIView* nav_;
};

class SubscriptionBarView {
public:
    SubscriptionBarView(CalendarVm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return bar_; }
private:
    UIView* bar_;
};

// ─── Top-level CalendarView ───────────────────────────────────────────────

class CalendarView {
public:
    CalendarView(CalendarVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::calendar::iosview
