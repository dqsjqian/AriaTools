#pragma once
//
// CalendarView — Qt view for the "calendar" module.
//
// Decomposed into sub-views, each owning its own widget + subscriptions:
//   MonthNavView       — prev/next/today/refresh buttons + month title
//   DayGridView       — 6×7 weekday header + day cells
//   SubscriptionBarView — URL input + subscribe button
//   SubscriptionListView — list of subscribed calendars (double-click remove)
//
// The top-level CalendarView assembles them into a vertical layout and
// wires the shared VM. register_calendar_view() is a thin entry point
// that constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

#include <array>
#include <memory>

class QListWidget;

namespace wb::calendar { class CalendarVm; struct DayCell; }

namespace wb::calendar::qtview {

// ─── Sub-views ────────────────────────────────────────────────────────────

class MonthNavView {
public:
    MonthNavView(CalendarVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return nav_; }
private:
    QWidget* nav_;
};

class DayGridView {
public:
    DayGridView(CalendarVm& vm, aria::binding::BindingEngine& be,
                std::vector<aria::Subscription>& subs);
    QWidget* widget() const { return grid_; }
private:
    struct DayWidgets { QFrame* frame; QLabel* dayLabel; QLabel* events; };
    QWidget* grid_;
    std::shared_ptr<std::array<DayWidgets, 42>> cells_;
    void paint_cell_(CalendarVm& vm, DayWidgets& w, const DayCell& c);
};

class SubscriptionBarView {
public:
    SubscriptionBarView(CalendarVm& vm, aria::binding::BindingEngine& be,
                        std::vector<aria::Subscription>& subs);
    QWidget* widget() const { return bar_; }
private:
    QWidget* bar_;
};

class SubscriptionListView {
public:
    SubscriptionListView(CalendarVm& vm, aria::binding::BindingEngine& be,
                         std::vector<aria::Subscription>& subs);
    QListWidget* widget() const { return list_; }
private:
    QListWidget* list_;
};

// ─── Top-level CalendarView ───────────────────────────────────────────────

class CalendarView {
public:
    CalendarView(CalendarVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
    std::unique_ptr<MonthNavView>          nav_;
    std::unique_ptr<DayGridView>           grid_;
    std::unique_ptr<SubscriptionBarView>   subBar_;
    std::unique_ptr<SubscriptionListView>  subList_;
};

}  // namespace wb::calendar::qtview
