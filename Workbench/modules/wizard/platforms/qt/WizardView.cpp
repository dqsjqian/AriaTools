
#include "support/UiHelpers.h"
#include "support/QtViewFactory.h"
#include "viewmodels/WizardVm.h"
#include "viewmodels/WizardVmHostVm.h"
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

/// Build a Qt label string from a wizard i18n key.
QString wiz_str(const char* key) {
    return QString::fromStdString(wb::i18n::str_in("wizard", key));
}

/// Rebuild the Step3 confirmation summary from the current draft
/// values + localized labels. Called on construction, on draft change
/// and on language change.
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

QWidget* build_step3_page(Step3Vm& vm) {
    auto* w = new QWidget;
    auto& subs = subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);
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
    subs.push_back(vm.finishedSummary.on_changed(sync));
    // Draft changes refresh the confirmation summary.
    subs.push_back(vm.draft->username.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    subs.push_back(vm.draft->email.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    subs.push_back(vm.draft->theme.on_changed(
        [summary, draft = vm.draft](const std::string&) {
            summary->setText(build_step3_summary(*draft));
        }));
    return w;
}

QWidget* build_step1_page(Step1Vm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto* lay = new QVBoxLayout(w);
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
    return w;
}

QWidget* build_step2_page(Step2Vm& vm) {
    auto* w = new QWidget;
    auto* lay = new QVBoxLayout(w);
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
    return w;
}
}  // namespace

static QWidget* build(WizardVmHostVm& host, aria::binding::BindingEngine& be) {
    auto& vm = host.inner();
    auto* w = new QWidget;
    auto& s_subs = subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(host.desc, view_for(info));

    auto* bar = new QHBoxLayout;
    auto* b1 = new QPushButton(wiz_str("nav1"));
    auto* b2 = new QPushButton(wiz_str("nav2"));
    auto* b3 = new QPushButton(wiz_str("nav3"));
    bar->addWidget(b1); bar->addWidget(b2); bar->addWidget(b3);
    lay->addLayout(bar);
    auto* stack = new QStackedWidget;
    auto* page1 = build_step1_page(*vm.step1, be);
    auto* page2 = build_step2_page(*vm.step2);
    auto* page3 = build_step3_page(*vm.step3);
    stack->addWidget(page1);
    stack->addWidget(page2);
    stack->addWidget(page3);
    lay->addWidget(stack, 1);
    auto* depthLbl = new QLabel;
    depthLbl->setStyleSheet("QLabel { color:#546e7a; font-size:11px; }");
    lay->addWidget(depthLbl);
    QObject::connect(b1, &QPushButton::clicked, [&vm, stack] { vm.toStep(1); stack->setCurrentIndex(0); });
    QObject::connect(b2, &QPushButton::clicked, [&vm, stack] { vm.toStep(2); stack->setCurrentIndex(1); });
    QObject::connect(b3, &QPushButton::clicked,
                     [&vm, stack, page3, &be]() mutable {
        auto* newPage = build_step3_page(*vm.step3);
        int idx = stack->indexOf(page3);
        if (idx >= 0) { stack->removeWidget(page3); page3->deleteLater(); }
        stack->insertWidget(2, newPage);
        page3 = newPage;
        vm.toStep(3);
        stack->setCurrentIndex(2);
        (void)be;
    });
    s_subs.push_back(vm.nav->current.on_changed(
        [depthLbl, &vm](const std::shared_ptr<aria::binding::ViewModel>&) {
            depthLbl->setText(QString("Navigator current depth = %1")
                              .arg(vm.nav->depth.get()));
        }));
    depthLbl->setText(QString("Navigator current depth = %1").arg(vm.nav->depth.get()));
    return w;
}
}  // namespace wb::wizard::qtview

namespace wb::wizard {
void register_wizard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "wizard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<WizardVmHostVm&>(vm), be);
        });
}
}  // namespace wb::wizard
