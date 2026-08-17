#include "WizardView.h"
#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/WizardVm.h"
#include "viewmodels/WizardVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace wb::wizard::qtview {

using namespace wb::ui;

namespace {

QString wiz_str(const char* key) {
    return QString::fromStdString(wb::i18n::str_in("wizard", key));
}
QString build_step3_summary(const WizardDraft& draft) {
    return wiz_str("summary")
        .arg(QString::fromStdString(draft.username.get()))
        .arg(QString::fromStdString(draft.email.get()))
        .arg(QString::fromStdString(wb::i18n::str_in(
            "wizard",
            draft.theme.get() == "Dark"      ? "theme_dark"
            : draft.theme.get() == "Solarized" ? "theme_solarized"
                                                : "theme_light")));
}

// ─── Step 1 account sub-view ───────────────────────────────────────────────
QWidget* build_step1(Step1Vm& vm, aria::binding::BindingEngine& be) {
    auto* w_ = new QWidget;
    auto* lay = new QVBoxLayout(w_);
    lay->addWidget(new QLabel("<h3>" + wiz_str("step1") + "</h3>"));
    auto* form = new QFormLayout;
    auto* user = new QLineEdit;
    auto* mail = new QLineEdit;
    form->addRow(wiz_str("username"), user);
    form->addRow(wiz_str("email"),    mail);
    lay->addLayout(form);
    lay->addStretch();
    be.bind_text(vm.draft->username, view_for(user));
    be.bind_text(vm.draft->email,    view_for(mail));
    return w_;
}

// ─── Step 2 theme sub-view ─────────────────────────────────────────────────
QWidget* build_step2(Step2Vm& vm, aria::binding::BindingEngine& be) {
    auto* w_ = new QWidget;
    auto* lay = new QVBoxLayout(w_);
    lay->addWidget(new QLabel("<h3>" + wiz_str("step2") + "</h3>"));
    auto* form = new QFormLayout;
    auto* themeBox = new QComboBox;
    themeBox->addItem(wiz_str("theme_light"),     QStringLiteral("Light"));
    themeBox->addItem(wiz_str("theme_dark"),      QStringLiteral("Dark"));
    themeBox->addItem(wiz_str("theme_solarized"), QStringLiteral("Solarized"));
    themeBox->setCurrentIndex(0);
    for (int i = 0; i < themeBox->count(); ++i) {
        if (themeBox->itemData(i).toString().toStdString() == vm.draft->theme.get()) {
            themeBox->setCurrentIndex(i);
            break;
        }
    }
    form->addRow(wiz_str("theme"), themeBox);
    lay->addLayout(form);
    lay->addStretch();
    QObject::connect(themeBox, &QComboBox::currentIndexChanged,
                     [themeBox, &vm](int) {
        vm.draft->theme.set(themeBox->currentData().toString().toStdString());
    });
    (void)be;
    return w_;
}

// ─── Step 3 confirm sub-view ───────────────────────────────────────────────
QWidget* build_step3(Step3Vm& vm, aria::binding::BindingEngine& be) {
    auto* w_ = new QWidget;
    auto& s_subs = subs_attached_to(w_);
    auto* lay = new QVBoxLayout(w_);
    lay->addWidget(new QLabel("<h3>" + wiz_str("step3") + "</h3>"));
    auto* summary = new QLabel;
    summary->setStyleSheet("QLabel { font-family:monospace; background:#f5f5f5;"
                           " padding:10px; border-radius:6px; }");
    summary->setText(build_step3_summary(*vm.draft));
    lay->addWidget(summary);
    auto* finishBtn = new QPushButton(wiz_str("finish"));
    lay->addWidget(finishBtn);
    auto* result = new QLabel;
    result->setText(wiz_str("unfinished"));
    lay->addWidget(result);
    lay->addStretch();
    QObject::connect(finishBtn, &QPushButton::clicked, [&vm] { vm.finish(); });
    auto sync = [result](const std::string& s) {
        if (!s.empty()) result->setText(QString::fromStdString(s));
    };
    s_subs.push_back(vm.finishedSummary.on_changed(sync));
    s_subs.push_back(vm.draft->username.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    s_subs.push_back(vm.draft->email.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    s_subs.push_back(vm.draft->theme.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    (void)be;
    return w_;
}

}  // namespace

// ─── Top-level build_view ──────────────────────────────────────────────────
QWidget* build_view(WizardVmHostVm& host, aria::binding::BindingEngine& be) {
    auto* root_ = new QWidget;
    auto& vm = host.inner();
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);

    // Hint banner: VM-owned desc property (i18n).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(host.desc, view_for(info));

    // Navigation bar.
    auto* bar = new QHBoxLayout;
    auto* b1 = new QPushButton(wiz_str("nav1"));
    auto* b2 = new QPushButton(wiz_str("nav2"));
    auto* b3 = new QPushButton(wiz_str("nav3"));
    bar->addWidget(b1); bar->addWidget(b2); bar->addWidget(b3);
    lay->addLayout(bar);

    // Step views (each owns its VM).
    auto* stack = new QStackedWidget;
    auto* step1 = build_step1(*vm.step1, be);
    auto* step2 = build_step2(*vm.step2, be);
    auto* step3 = build_step3(*vm.step3, be);
    stack->addWidget(step1);
    stack->addWidget(step2);
    stack->addWidget(step3);
    lay->addWidget(stack, 1);

    auto* depthLbl = new QLabel;
    depthLbl->setStyleSheet("QLabel { color:#546e7a; font-size:11px; }");
    lay->addWidget(depthLbl);

    QObject::connect(b1, &QPushButton::clicked, [&vm, stack] { vm.toStep(1); stack->setCurrentIndex(0); });
    QObject::connect(b2, &QPushButton::clicked, [&vm, stack] { vm.toStep(2); stack->setCurrentIndex(1); });
    QObject::connect(b3, &QPushButton::clicked,
                     [&vm, stack, &step3, &be]() {
        // Rebuild a fresh confirm view each time (resets its local state).
        auto* newStep3 = build_step3(*vm.step3, be);
        int idx = stack->indexOf(step3);
        if (idx >= 0) { stack->removeWidget(step3); step3->deleteLater(); }
        stack->insertWidget(2, newStep3);
        step3 = newStep3;
        vm.toStep(3);
        stack->setCurrentIndex(2);
    });

    s_subs.push_back(vm.nav->current.on_changed(
        [depthLbl, &vm](const std::shared_ptr<aria::binding::ViewModel>&) {
            depthLbl->setText(QString("Navigator current depth = %1")
                              .arg(vm.nav->depth.get()));
        }));
    depthLbl->setText(QString("Navigator current depth = %1").arg(vm.nav->depth.get()));
    return root_;
}

}  // namespace wb::wizard::qtview

namespace wb::wizard {
void register_wizard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "wizard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<WizardVmHostVm&>(vm);
            return qtview::build_view(host, be);
        });
}
}  // namespace wb::wizard
