#pragma once
//
// DashboardView — Qt view for the "dashboard" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_dashboard_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::dashboard { class DashboardVm; }

namespace wb::dashboard::qtview {

QWidget* build_view(DashboardVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::dashboard::qtview
