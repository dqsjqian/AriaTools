#include "UnitConvertView.h"
#include "support/UiHelpers.h"
#include "viewmodels/UnitConvertVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace wb::unitconvert::qtview {
using namespace wb::ui;

UnitConvertView::UnitConvertView(UnitConvertVm& host, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& vm = host;
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(host.desc, view_for(info));

    auto* cat = new QComboBox;
    cat->addItem(QString::fromStdString(wb::i18n::str_in("unitconvert", "cat_temperature")));
    cat->addItem(QString::fromStdString(wb::i18n::str_in("unitconvert", "cat_length")));
    cat->addItem(QString::fromStdString(wb::i18n::str_in("unitconvert", "cat_weight")));
    lay->addWidget(cat);
    auto* valSpin = new QDoubleSpinBox;
    valSpin->setRange(-1000.0, 1000000.0);
    valSpin->setDecimals(3);
    auto* fromLbl = new QLabel;
    auto* fromRow = new QHBoxLayout;
    fromRow->addWidget(new QLabel(QString::fromStdString(wb::i18n::str_in("unitconvert", "input"))));
    fromRow->addWidget(valSpin);
    fromRow->addWidget(fromLbl);
    lay->addLayout(fromRow);
    auto* out = new QLabel;
    lay->addWidget(out);
    lay->addStretch();
    be.bind_double(vm.value, view_for(valSpin));
    QObject::connect(cat, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [&vm](int idx) { vm.category.set(static_cast<Category>(idx)); });
    auto syncResult = [out, &vm] {
        out->setText(QString(QString::fromStdString(wb::i18n::str_in("unitconvert", "equals")))
            .arg(vm.converted.get(), 0, 'f', 3)
            .arg(QString::fromStdString(vm.toLabel.get())));
    };
    auto syncFrom = [fromLbl](const std::string& s) { fromLbl->setText(QString::fromStdString(s)); };
    syncFrom(vm.fromLabel.get());
    syncResult();
    s_subs.push_back(vm.fromLabel .on_changed(syncFrom));
    s_subs.push_back(vm.converted .on_changed([syncResult](double){ syncResult(); }));
    s_subs.push_back(vm.toLabel   .on_changed([syncResult](const std::string&){ syncResult(); }));
}

}  // namespace wb::unitconvert::qtview
