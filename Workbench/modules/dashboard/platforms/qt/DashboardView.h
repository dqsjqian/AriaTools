#pragma once
//
// DashboardView — Qt view for the "dashboard" module.
//
// Hosts the cross-module navigation demo: the "open cart" buttons fire the
// VM's Commands (routing decision lives in the VM layer), and this view
// renders `nav->current` — pushing a NavigationEntryVm presents the target
// module's view according to its Presentation kind:
//   Push   -> embedded in the page container (QStackedWidget)
//   Modal  -> modal QDialog overlay
//   Window -> standalone top-level QWidget window
// Closing a modal/window fires the VM's navBack Command (Pop), so the
// navigation stack stays in sync with what the user sees.
//
#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <QDialog>
#include <QPointer>
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
    QWidget* pageHost_;      ///< embedded navigation target
    QPointer<QDialog> modal_;   ///< live modal dialog (Modal kind)
    QPointer<QWidget> window_;  ///< live standalone window (Window kind)
    /// True while re-rendering: tearing down an old modal/window must NOT
    /// trigger a navigation Pop (it is being replaced, not user-closed).
    bool navSettling_ = false;
    std::vector<aria::Subscription> subs_;
};

}  // namespace wb::dashboard::qtview
