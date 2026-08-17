#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/TipCalcVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace wb::tipcalc::qtview {

static QWidget* build(TipCalcVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& s_subs = wb::ui::subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);

    // ── Widgets ──────────────────────────────────────────────────────────
    auto* titleLbl = wb::ui::make_title("");
    auto* descLbl  = wb::ui::make_info("");
    lay->addWidget(titleLbl);
    lay->addWidget(descLbl);

    auto* form = new QFormLayout;
    auto* billLabel = new QLabel;
    auto* tipLabel  = new QLabel;
    auto* peopleLabel = new QLabel;
    auto* billSpin = new QDoubleSpinBox;
    billSpin->setRange(0.0, 99999.0);
    billSpin->setDecimals(2);
    billSpin->setPrefix("¥ ");
    auto* tipSlider = new QSlider(Qt::Horizontal);
    tipSlider->setRange(0, 30);
    auto* tipValueLbl = new QLabel;
    auto* tipRow = new QHBoxLayout;
    tipRow->addWidget(tipSlider);
    tipRow->addWidget(tipValueLbl);
    auto* peopleSpin = new QSpinBox;
    peopleSpin->setRange(1, 50);
    form->addRow(billLabel, billSpin);
    form->addRow(tipLabel, tipRow);
    form->addRow(peopleLabel, peopleSpin);
    lay->addLayout(form);

    auto* tipLbl   = new QLabel;
    auto* totalLbl = new QLabel;
    auto* perLbl   = new QLabel;
    lay->addWidget(tipLbl);
    lay->addWidget(totalLbl);
    lay->addWidget(perLbl);

    auto* roundBtn = new QPushButton;
    lay->addWidget(roundBtn);
    lay->addStretch();

    // ── Bindings (VM is the single source of truth; View renders only) ────
    be.bind_text_oneway(vm.title, wb::ui::view_for(titleLbl));
    be.bind_text_oneway(vm.desc,  wb::ui::view_for(descLbl));
    be.bind_double(vm.bill,       wb::ui::view_for(billSpin));
    be.bind_int   (vm.tipPercent, wb::ui::view_for(tipSlider));
    be.bind_int   (vm.people,     wb::ui::view_for(peopleSpin));
    be.bind_command(vm.roundUp,   wb::ui::view_for(roundBtn));

    // Form captions + button text from i18n properties.
    be.bind_text_oneway(vm.billLabel, wb::ui::view_for(billLabel));
    be.bind_text_oneway(vm.tipLabel,  wb::ui::view_for(tipLabel));
    be.bind_text_oneway(vm.peopleLabel, wb::ui::view_for(peopleLabel));
    s_subs.push_back(vm.roundUpText.on_changed(
        [roundBtn](const std::string& t) { roundBtn->setText(QString::fromStdString(t)); }));

    // Result rows: caption (i18n) + formatted value.
    auto fmt = [](double x) { return QString::number(x, 'f', 2); };
    auto syncTip = [tipLbl, fmt, &vm](double v) {
        tipLbl->setText(QString::fromStdString(vm.tipAmountText.get()) + ": ¥ " + fmt(v));
    };
    auto syncTotal = [totalLbl, fmt, &vm](double v) {
        totalLbl->setText(QString::fromStdString(vm.totalText.get()) + ": ¥ " + fmt(v));
    };
    auto syncPer = [perLbl, fmt, &vm](double v) {
        perLbl->setText(QString::fromStdString(vm.perPersonText.get()) + ": ¥ " + fmt(v));
    };
    auto syncPct = [tipValueLbl](int p) { tipValueLbl->setText(QString::number(p) + " %"); };

    syncTip (vm.tipAmount.get());
    syncTotal(vm.total.get());
    syncPer  (vm.perPerson.get());
    syncPct  (vm.tipPercent.get());

    s_subs.push_back(vm.tipAmount .on_changed(syncTip));
    s_subs.push_back(vm.total     .on_changed(syncTotal));
    s_subs.push_back(vm.perPerson .on_changed(syncPer));
    s_subs.push_back(vm.tipPercent.on_changed(syncPct));
    // Caption changes re-render the row too.
    s_subs.push_back(vm.tipAmountText.on_changed([&vm, syncTip](const std::string&) { syncTip(vm.tipAmount.get()); }));
    s_subs.push_back(vm.totalText.on_changed([&vm, syncTotal](const std::string&) { syncTotal(vm.total.get()); }));
    s_subs.push_back(vm.perPersonText.on_changed([&vm, syncPer](const std::string&) { syncPer(vm.perPerson.get()); }));

    return w;
}

}  // namespace wb::tipcalc::qtview

namespace wb::tipcalc {

void register_tipcalc_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "tipcalc",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<TipCalcVm&>(vm), be);
        });
}

}  // namespace wb::tipcalc
