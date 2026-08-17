#include "DashboardView.h"
#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/DashboardVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace wb::dashboard::qtview {

QWidget* build_view(DashboardVm& vm, aria::binding::BindingEngine& be) {
    auto* root_ = new QWidget;
    auto* lay = new QVBoxLayout(root_);

    auto* title = wb::ui::make_title("");
    auto* info  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(info);

    // ── Cross-module live state ───────────────────────────────────────
    // Cart badge + last order — driven by EventBus events published by the
    // cart module (no direct coupling). See DashboardVm's subscriptions.
    auto* cartBadgeLbl = new QLabel;
    // Explicit dark text on the light badge — without `color:` the label
    // inherits the system's light text in dark theme, making it illegible
    // on the #fff3e0 background.
    cartBadgeLbl->setStyleSheet(
        "QLabel { background:#fff3e0; color:#633806;"
        " border:1px solid #ffb74d; border-radius:12px;"
        " padding:6px 12px; font-weight:bold; }");
    lay->addWidget(cartBadgeLbl);
    be.bind_text_oneway(vm.cartBadge, wb::ui::view_for(cartBadgeLbl));

    auto* orderLbl = new QLabel;
    // Dark-theme-friendly green (brighter than #2e7d32 which vanishes on
    // dark backgrounds).
    orderLbl->setStyleSheet("QLabel { color:#66bb6a; padding:4px; }");
    orderLbl->setWordWrap(true);
    lay->addWidget(orderLbl);
    be.bind_text_oneway(vm.lastOrder, wb::ui::view_for(orderLbl));

    lay->addStretch();

    be.bind_text_oneway(vm.welcome, wb::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ui::view_for(info));
    return root_;
}

}  // namespace wb::dashboard::qtview

namespace wb::dashboard {

void register_dashboard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "dashboard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build_view(static_cast<DashboardVm&>(vm), be);
        });
}

}  // namespace wb::dashboard
