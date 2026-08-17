#include "DashboardView.h"
#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/DashboardVm.h"

#include "module_api/NavigationEntryVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace wb::dashboard::qtview {

DashboardView::DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget), pageHost_(new QWidget) {
    auto* lay = new QVBoxLayout(root_);

    auto* title = wb::ui::make_title("");
    auto* info  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(info);

    // ── Cross-module live state ───────────────────────────────────────
    auto* cartBadgeLbl = new QLabel;
    cartBadgeLbl->setStyleSheet(
        "QLabel { background:#fff3e0; color:#633806;"
        " border:1px solid #ffb74d; border-radius:12px;"
        " padding:6px 12px; font-weight:bold; }");
    lay->addWidget(cartBadgeLbl);
    be.bind_text_oneway(vm.cartBadge, wb::ui::view_for(cartBadgeLbl));

    auto* orderLbl = new QLabel;
    orderLbl->setStyleSheet("QLabel { color:#66bb6a; padding:4px; }");
    orderLbl->setWordWrap(true);
    lay->addWidget(orderLbl);
    be.bind_text_oneway(vm.lastOrder, wb::ui::view_for(orderLbl));

    // ── Cross-module navigation (VM-layer routing) ────────────────────
    // The View only fires Commands; the VM decides which module page to
    // push. This view renders the navigator's current entry.
    auto* openCartBtn = new QPushButton(root_);
    auto* backBtn     = new QPushButton(root_);
    auto* navRow      = new QHBoxLayout;
    navRow->addWidget(openCartBtn);
    navRow->addWidget(backBtn);
    navRow->addStretch();
    lay->addLayout(navRow);
    be.bind_text_oneway(vm.openCartLabel, wb::ui::view_for(openCartBtn));
    be.bind_text_oneway(vm.navBackLabel,  wb::ui::view_for(backBtn));
    be.bind_command(vm.openCart, wb::ui::view_for(openCartBtn));
    be.bind_command(vm.navBack,  wb::ui::view_for(backBtn));

    // Embedded page host: renders the current navigation entry.
    auto* stack = new QStackedWidget;
    stack->addWidget(pageHost_);
    lay->addWidget(stack, 1);

    auto& navCurrent = vm.navigator().current();
    auto render_current = [this, &vm, &be, stack](const std::shared_ptr<aria::binding::ViewModel>& current) {
        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(current.get());
        if (!entry) {
            // At root: show the default placeholder.
            stack->setCurrentWidget(pageHost_);
            return;
        }
        auto* page = wb::qt::QtViewFactory::instance().build(
            entry->module_id(), entry->inner(), be);
        if (page) {
            stack->addWidget(page);
            stack->setCurrentWidget(page);
        }
    };
    subs_.push_back(navCurrent.on_changed(
        [render_current](const std::shared_ptr<aria::binding::ViewModel>& c) {
            render_current(c);
        }));
    render_current(navCurrent.get());

    lay->addStretch();

    be.bind_text_oneway(vm.welcome, wb::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ui::view_for(info));
}

}  // namespace wb::dashboard::qtview
