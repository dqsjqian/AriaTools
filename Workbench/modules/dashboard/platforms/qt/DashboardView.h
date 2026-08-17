#pragma once
//
// DashboardView — Qt view for the "dashboard" module.
//
// Hosts the cross-module navigation demo: the "open cart" button fires the
// VM's openCart Command (routing decision lives in the VM layer), and this
// view merely renders `nav->current` — pushing a NavigationEntryVm swaps
// the embedded page container to the target module's view.
//
#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <QWidget>

#include <vector>

namespace wb::dashboard { class DashboardVm; }

namespace wb::dashboard::qtview {

class DashboardView {
public:
    DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }

private:
    QWidget* root_;
    QWidget* pageHost_;                 ///< embedded navigation target
    std::vector<aria::Subscription> subs_;
};

}  // namespace wb::dashboard::qtview
