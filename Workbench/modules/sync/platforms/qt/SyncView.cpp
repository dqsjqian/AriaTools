#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/SyncVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::sync::qtview {

static QWidget* build(wb::sync::SyncVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);

    auto* row = new QHBoxLayout;
    auto* syncBtn = new QPushButton;
    auto* pullBtn = new QPushButton;
    auto* pushBtn = new QPushButton;
    row->addWidget(syncBtn);
    row->addWidget(pullBtn);
    row->addWidget(pushBtn);
    row->addStretch();
    lay->addLayout(row);

    auto* statusLbl = new QLabel;
    statusLbl->setStyleSheet("QLabel { font-weight:bold; }");
    lay->addWidget(statusLbl);

    auto* logView = new QPlainTextEdit;
    logView->setReadOnly(true);
    lay->addWidget(logView, 1);

    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));
    be.bind_text_oneway(vm.syncLabel, wb::ui::view_for(syncBtn));
    be.bind_text_oneway(vm.pullLabel, wb::ui::view_for(pullBtn));
    be.bind_text_oneway(vm.pushLabel, wb::ui::view_for(pushBtn));
    be.bind_command(vm.syncNow, wb::ui::view_for(syncBtn));
    be.bind_command(vm.pullOnly, wb::ui::view_for(pullBtn));
    be.bind_command(vm.pushOnly, wb::ui::view_for(pushBtn));
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
    be.bind_text_oneway(vm.log, wb::ui::view_for(logView));

    return w;
}

}  // namespace wb::sync::qtview

namespace wb::sync {

void register_sync_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "sync",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<SyncVm&>(vm), be);
        });
}

}  // namespace wb::sync
