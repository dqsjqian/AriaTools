#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/SettingsVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace wb::settings::qtview {

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
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(10);

    // Language selector: switching refreshes immediately (via the VM's switchLanguage command).
    auto* langBox = new QComboBox;
    langBox->addItem("简体中文", "zh-CN");
    langBox->addItem("English", "en");
    for (int i = 0; i < langBox->count(); ++i)
        if (langBox->itemData(i).toString().toStdString() == vm.language.get())
            langBox->setCurrentIndex(i);
    QObject::connect(langBox, &QComboBox::currentIndexChanged, [langBox, &vm](int) {
        vm.switchLanguage.execute(langBox->currentData().toString().toStdString());
    });

    form->addRow(row_label(vm.languageLabel, subs), langBox);
    lay->addLayout(form);
    lay->addStretch();

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
