#include "DashboardView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/DashboardVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace wb::dashboard::qtview {

DashboardView::DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto* lay = new QVBoxLayout(root_);

    auto* title = wb::ui::make_title("");
    auto* info  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(info);

    // ── Cross-module live state ───────────────────────────────────────
    // Cart badge + last order — driven by EventBus events published by the
    // cart module (no direct coupling). See DashboardVm's subscriptions.
    auto* cartBadgeLbl = new QLabel;
    cartBadgeLbl->setStyleSheet(
        "QLabel { background:#fff3e0; border:1px solid #ffb74d;"
        " border-radius:12px; padding:6px 12px; font-weight:bold; }");
    lay->addWidget(cartBadgeLbl);
    be.bind_text_oneway(vm.cartBadge, wb::ui::view_for(cartBadgeLbl));

    auto* orderLbl = new QLabel;
    orderLbl->setStyleSheet("QLabel { color:#2e7d32; padding:4px; }");
    orderLbl->setWordWrap(true);
    lay->addWidget(orderLbl);
    be.bind_text_oneway(vm.lastOrder, wb::ui::view_for(orderLbl));

    lay->addStretch();

    be.bind_text_oneway(vm.welcome, wb::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ui::view_for(info));
}

}  // namespace wb::dashboard::qtview

namespace wb::dashboard {

void register_dashboard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "dashboard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::DashboardView(static_cast<DashboardVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::dashboard
