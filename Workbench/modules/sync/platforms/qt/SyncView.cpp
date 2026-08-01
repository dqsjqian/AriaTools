#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/SyncVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::sync::qtview {

// Form row labels: bound to VM text (updates on language change).
static QLabel* row_label(aria::Property<std::string>& p,
                         std::vector<aria::Subscription>& subs) {
    auto* l = new QLabel(QString::fromStdString(p.get()));
    subs.push_back(p.on_changed([l](const std::string& s) {
        l->setText(QString::fromStdString(s));
    }));
    return l;
}

static QWidget* build(wb::sync::SyncVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& subs = wb::ui::subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));

    // ── Remote repository configuration form ──
    auto* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(10);

    auto makeEdit = [] {
        auto* e = new QLineEdit;
        e->setMinimumHeight(32);
        e->setClearButtonEnabled(true);
        return e;
    };
    auto* dataDir = makeEdit();
    auto* remote  = makeEdit();
    auto* branch  = makeEdit();
    auto* user    = makeEdit();
    auto* token   = makeEdit(); token->setEchoMode(QLineEdit::Password);
    auto* autoSync = new QCheckBox;

    form->addRow(row_label(vm.dataDirLabel, subs), dataDir);
    form->addRow(row_label(vm.remoteLabel, subs), remote);
    form->addRow(row_label(vm.branchLabel, subs), branch);
    form->addRow(row_label(vm.usernameLabel, subs), user);
    form->addRow(row_label(vm.tokenLabel, subs), token);
    form->addRow(QString(), autoSync);
    lay->addLayout(form);

    be.bind_text_oneway(vm.autoSyncLabel, wb::ui::view_for(autoSync));
    be.bind_text(vm.dataDir,   wb::ui::view_for(dataDir));
    be.bind_text(vm.remoteUrl, wb::ui::view_for(remote));
    be.bind_text(vm.branch,    wb::ui::view_for(branch));
    be.bind_text(vm.username,  wb::ui::view_for(user));
    be.bind_text(vm.token,     wb::ui::view_for(token));
    be.bind_bool(vm.autoSync,  wb::ui::view_for(autoSync));

    auto* saveBtn = new QPushButton;
    lay->addWidget(saveBtn);
    be.bind_text_oneway(vm.saveLabel, wb::ui::view_for(saveBtn));
    be.bind_command(vm.saveConfig, wb::ui::view_for(saveBtn));

    // ── Sync actions ──
    auto* row = new QHBoxLayout;
    auto* syncBtn = new QPushButton;
    auto* pullBtn = new QPushButton;
    auto* pushBtn = new QPushButton;
    row->addWidget(syncBtn);
    row->addWidget(pullBtn);
    row->addWidget(pushBtn);
    row->addStretch();
    lay->addLayout(row);
    be.bind_text_oneway(vm.syncLabel, wb::ui::view_for(syncBtn));
    be.bind_text_oneway(vm.pullLabel, wb::ui::view_for(pullBtn));
    be.bind_text_oneway(vm.pushLabel, wb::ui::view_for(pushBtn));
    be.bind_command(vm.syncNow, wb::ui::view_for(syncBtn));
    be.bind_command(vm.pullOnly, wb::ui::view_for(pullBtn));
    be.bind_command(vm.pushOnly, wb::ui::view_for(pushBtn));

    auto* statusLbl = new QLabel;
    statusLbl->setStyleSheet("QLabel { font-weight:bold; }");
    lay->addWidget(statusLbl);
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));

    auto* logView = new QPlainTextEdit;
    logView->setReadOnly(true);
    lay->addWidget(logView, 1);
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
