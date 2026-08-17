#pragma once
//
// CalendarView — Qt view for the "calendar" module (Aria free-function view).
//
// Decomposed into sub-views, each owning its own widget + subscriptions:
//   month navigation  — prev/next/today/refresh buttons + month title
//   day grid          — 6×7 weekday header + day cells
//   subscription bar  — URL input + subscribe button
//   subscription list — list of subscribed calendars (double-click remove)
//
// The top-level build_view() assembles them into a vertical layout and wires
// the shared VM. register_calendar_view() registers the builder with the
// QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::calendar { class CalendarVm; }

namespace wb::calendar::qtview {

QWidget* build_view(CalendarVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::calendar::qtview
