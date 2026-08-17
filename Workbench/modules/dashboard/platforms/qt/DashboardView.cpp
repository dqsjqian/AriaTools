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

    // ── Extension point (mount) status ─────────────────────────────────
    auto* mountStatusLbl = new QLabel;
    mountStatusLbl->setStyleSheet(
        "QLabel { color:#534AB7; padding:4px; font-weight:bold; }");
    lay->addWidget(mountStatusLbl);
    be.bind_text_oneway(vm.mountStatus, wb::ui::view_for(mountStatusLbl));

    // ── Cross-module navigation (VM-layer routing) ────────────────────
    // The View only fires Commands; the VM decides which module page to
    // push AND how to present it (modal / window). This view renders the
    // navigator's current entry by its Presentation kind.
    auto* modalCartBtn   = new QPushButton(root_);
    auto* windowCartBtn  = new QPushButton(root_);
    auto* backBtn        = new QPushButton(root_);
    auto* mountToggleBtn = new QPushButton(root_);
    auto* navRow         = new QHBoxLayout;
    navRow->addWidget(modalCartBtn);
    navRow->addWidget(windowCartBtn);
    navRow->addWidget(backBtn);
    navRow->addWidget(mountToggleBtn);
    navRow->addStretch();
    lay->addLayout(navRow);
    be.bind_text_oneway(vm.modalCartLabel,   wb::ui::view_for(modalCartBtn));
    be.bind_text_oneway(vm.windowCartLabel,  wb::ui::view_for(windowCartBtn));
    be.bind_text_oneway(vm.navBackLabel,     wb::ui::view_for(backBtn));
    be.bind_text_oneway(vm.mountToggleLabel, wb::ui::view_for(mountToggleBtn));
    be.bind_command(vm.modalCart,   wb::ui::view_for(modalCartBtn));
    be.bind_command(vm.windowCart,  wb::ui::view_for(windowCartBtn));
    be.bind_command(vm.navBack,     wb::ui::view_for(backBtn));
    be.bind_command(vm.mountToggle, wb::ui::view_for(mountToggleBtn));

    // Embedded page host: renders the EXTENSION POINT content — the mounted
    // provider's UI (e.g. cart) fills this area, or a placeholder when the
    // slot is empty. Distinct from navigation (modal/window) which renders
    // above the whole page.
    auto* stack = new QStackedWidget;
    stack->addWidget(pageHost_);
    lay->addWidget(stack, 1);

    auto* placeholder = new QLabel;
    placeholder->setWordWrap(true);
    placeholder->setStyleSheet("QLabel { color:#888780; padding:12px; }");
    pageHost_->setLayout([&] {
        auto* l = new QVBoxLayout(pageHost_);
        l->addWidget(placeholder);
        l->addStretch();
        return l;
    }());

    auto render_mount = [this, &vm, &be, stack, placeholder](
                            const std::shared_ptr<aria::binding::ViewModel>& vmp) {
        // Tear down the previous mounted UI (placeholder or provider page).
        stack->setCurrentWidget(pageHost_);
        while (stack->count() > 1) {
            QWidget* w = stack->widget(1);
            stack->removeWidget(w);
            delete w;
        }
        if (!vmp) {
            placeholder->setText(QString::fromStdString(vm.mountStatus.get()));
            stack->setCurrentWidget(pageHost_);
            return;
        }
        // Build the provider's view from the resolved VM.
        auto* page = wb::qt::QtViewFactory::instance().build(
            vm.mountedModule.get(), *vmp, be);
        if (page) {
            stack->addWidget(page);
            stack->setCurrentWidget(page);
        }
    };
    subs_.push_back(vm.mountedVm.on_changed(
        [render_mount](const std::shared_ptr<aria::binding::ViewModel>& v) {
            render_mount(v);
        }));
    render_mount(vm.mountedVm.get());

    // Navigation (modal/window) rendering — independent of the mount area.
    auto& navCurrent = vm.navigator().current();
    auto render_nav = [this, &vm, &be](const std::shared_ptr<aria::binding::ViewModel>& current) {
        // Re-rendering (navigation changed): tear down any live modal/window
        // WITHOUT popping the stack — they are being replaced, not closed by
        // the user. The navSettling_ guard silences their close handlers.
        navSettling_ = true;
        if (modal_) { modal_->close(); modal_->deleteLater(); modal_ = nullptr; }
        if (window_) { window_->close(); window_->deleteLater(); window_ = nullptr; }
        navSettling_ = false;

        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(current.get());
        if (!entry) return;  // at root: nothing to present above the page

        auto* page = wb::qt::QtViewFactory::instance().build(
            entry->module_id(), entry->inner(), be);
        if (!page) return;

        switch (entry->presentation()) {
            case wb::module_api::Presentation::Push:
                // Reserved: Push (embedded) is superseded by the extension
                // point system (MountRegistry). Modal/Window remain nav.
                delete page;
                break;
            case wb::module_api::Presentation::Modal: {
                auto* dlg = new QDialog(root_);
                dlg->setModal(true);
                dlg->setWindowTitle(QString::fromStdString(entry->module_id()));
                auto* dlay = new QVBoxLayout(dlg);
                dlay->setContentsMargins(0, 0, 0, 0);
                dlay->addWidget(page);
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
        [render_nav](const std::shared_ptr<aria::binding::ViewModel>& c) {
            render_nav(c);
        }));
    render_nav(navCurrent.get());

    lay->addStretch();

    be.bind_text_oneway(vm.welcome, wb::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ui::view_for(info));
}

}  // namespace wb::dashboard::qtview
