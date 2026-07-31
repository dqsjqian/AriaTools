#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/SettingsVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::settings::qtview {

// 表单行标签：返回一个绑定到 VM 文案的 QLabel（随语言更新）。
static QLabel* row_label(aria::Property<std::string>& p,
                         std::vector<aria::Subscription>& subs) {
    auto* l = new QLabel(QString::fromStdString(p.get()));
    subs.push_back(p.on_changed([l](const std::string& s) {
        l->setText(QString::fromStdString(s));
    }));
    return l;
}

static QWidget* build(wb::settings::SettingsVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& subs = wb::ui::subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));

    auto* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::DontWrapRows);
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

    // 语言选择器：切换即时刷新（走 VM switchLanguage 命令）。
    auto* langBox = new QComboBox;
    langBox->addItem("简体中文", "zh-CN");
    langBox->addItem("English", "en");
    for (int i = 0; i < langBox->count(); ++i)
        if (langBox->itemData(i).toString().toStdString() == vm.language.get())
            langBox->setCurrentIndex(i);
    QObject::connect(langBox, &QComboBox::currentIndexChanged, [langBox, &vm](int) {
        vm.switchLanguage.execute(langBox->currentData().toString().toStdString());
    });

    form->addRow(row_label(vm.dataDirLabel, subs), dataDir);
    form->addRow(row_label(vm.remoteLabel, subs), remote);
    form->addRow(row_label(vm.branchLabel, subs), branch);
    form->addRow(row_label(vm.usernameLabel, subs), user);
    form->addRow(row_label(vm.tokenLabel, subs), token);
    form->addRow(row_label(vm.languageLabel, subs), langBox);
    form->addRow(QString(), autoSync);
    lay->addLayout(form);

    // autoSync 文案（QCheckBox 是文本控件）。
    be.bind_text_oneway(vm.autoSyncLabel, wb::ui::view_for(autoSync));

    auto* saveBtn = new QPushButton;
    lay->addWidget(saveBtn);
    be.bind_text_oneway(vm.saveLabel, wb::ui::view_for(saveBtn));

    auto* statusLbl = new QLabel;
    statusLbl->setWordWrap(true);
    lay->addWidget(statusLbl);
    lay->addStretch();

    be.bind_text(vm.dataDir,   wb::ui::view_for(dataDir));
    be.bind_text(vm.remoteUrl, wb::ui::view_for(remote));
    be.bind_text(vm.branch,    wb::ui::view_for(branch));
    be.bind_text(vm.username,  wb::ui::view_for(user));
    be.bind_text(vm.token,     wb::ui::view_for(token));
    be.bind_bool(vm.autoSync,  wb::ui::view_for(autoSync));
    be.bind_command(vm.save,   wb::ui::view_for(saveBtn));
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));

    return w;
}

}  // namespace wb::settings::qtview

namespace wb::settings {

void register_settings_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "settings",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<SettingsVm&>(vm), be);
        });
}

}  // namespace wb::settings
