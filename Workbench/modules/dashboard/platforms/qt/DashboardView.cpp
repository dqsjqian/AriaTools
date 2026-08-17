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
    // push AND how to present it (embedded / modal / window). This view
    // renders the navigator's current entry by its Presentation kind.
    auto* openCartBtn    = new QPushButton(root_);
    auto* modalCartBtn   = new QPushButton(root_);
    auto* windowCartBtn  = new QPushButton(root_);
    auto* backBtn        = new QPushButton(root_);
    auto* navRow         = new QHBoxLayout;
    navRow->addWidget(openCartBtn);
    navRow->addWidget(modalCartBtn);
    navRow->addWidget(windowCartBtn);
    navRow->addWidget(backBtn);
    navRow->addStretch();
    lay->addLayout(navRow);
    be.bind_text_oneway(vm.openCartLabel,   wb::ui::view_for(openCartBtn));
    be.bind_text_oneway(vm.modalCartLabel,  wb::ui::view_for(modalCartBtn));
    be.bind_text_oneway(vm.windowCartLabel, wb::ui::view_for(windowCartBtn));
    be.bind_text_oneway(vm.navBackLabel,    wb::ui::view_for(backBtn));
    be.bind_command(vm.openCart,   wb::ui::view_for(openCartBtn));
    be.bind_command(vm.modalCart,  wb::ui::view_for(modalCartBtn));
    be.bind_command(vm.windowCart, wb::ui::view_for(windowCartBtn));
    be.bind_command(vm.navBack,    wb::ui::view_for(backBtn));

    // Embedded page host: renders the current navigation entry.
    auto* stack = new QStackedWidget;
    stack->addWidget(pageHost_);
    lay->addWidget(stack, 1);

    auto& navCurrent = vm.navigator().current();
    auto render_current = [this, &vm, &be, stack](const std::shared_ptr<aria::binding::ViewModel>& current) {
        // Re-rendering (navigation changed): tear down any live modal/window
        // WITHOUT popping the stack — they are being replaced, not closed by
        // the user. The navSettling_ guard silences their close handlers.
        navSettling_ = true;
        if (modal_) { modal_->close(); modal_->deleteLater(); modal_ = nullptr; }
        if (window_) { window_->close(); window_->deleteLater(); window_ = nullptr; }
        navSettling_ = false;

        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(current.get());
        if (!entry) {
            // At root: show the default placeholder.
            stack->setCurrentWidget(pageHost_);
            return;
        }
        auto* page = wb::qt::QtViewFactory::instance().build(
            entry->module_id(), entry->inner(), be);
        if (!page) return;

        switch (entry->presentation()) {
            case wb::module_api::Presentation::Push: {
                // Embedded page: swap into the host stack (drop the previous
                // embedded page so repeated pushes don't accumulate).
                stack->setCurrentWidget(pageHost_);
                while (stack->count() > 1) {
                    QWidget* w = stack->widget(1);
                    stack->removeWidget(w);
                    delete w;
                }
                stack->addWidget(page);
                stack->setCurrentWidget(page);
                break;
            }
            case wb::module_api::Presentation::Modal: {
                auto* dlg = new QDialog(root_);
                dlg->setModal(true);
                dlg->setWindowTitle(QString::fromStdString(entry->module_id()));
                auto* dlay = new QVBoxLayout(dlg);
                dlay->setContentsMargins(0, 0, 0, 0);
                dlay->addWidget(page);
                // User closes the dialog -> pop the stack entry (guarded so
                // our teardown above doesn't fire it).
                QObject::connect(dlg, &QDialog::finished, dlg,
                                 [this, &vm](int) {
                                     if (!navSettling_) vm.navBack.execute();
                                 });
                dlg->resize(480, 360);
                dlg->show();
                modal_ = dlg;
                break;
            }
            case wb::module_api::Presentation::Window: {
                auto* win = new QWidget(nullptr, Qt::Window);
                win->setAttribute(Qt::WA_DeleteOnClose);
                win->setWindowTitle(QString::fromStdString(entry->module_id()));
                auto* wlay = new QVBoxLayout(win);
                wlay->setContentsMargins(0, 0, 0, 0);
                wlay->addWidget(page);
                // User closes the window -> pop the stack entry. WA_DeleteOnClose
                // destroys it, firing `destroyed`; the guard skips it when we are
                // simply re-rendering.
                QObject::connect(win, &QObject::destroyed, win,
                                 [this, &vm]() {
                                     if (!navSettling_) vm.navBack.execute();
                                 });
                win->resize(520, 420);
                win->show();
                window_ = win;
                break;
            }
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
