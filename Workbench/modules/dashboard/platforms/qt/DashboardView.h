#pragma once
//
// DashboardView — Qt view for the "dashboard" module.
//
// register_dashboard_view() is a thin entry point that constructs the view
// and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::dashboard { class DashboardVm; }

namespace wb::dashboard::qtview {

class DashboardView {
public:
    DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::dashboard::qtview
