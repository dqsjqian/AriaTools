
#include "support/UiHelpers.h"
#include "support/QtViewFactory.h"
#include "viewmodels/LoginVm.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
namespace wb::login::qtview {
using namespace wb::ui;
static QWidget* build(LoginVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& s_subs = subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(vm.desc, view_for(info));

    auto* form = new QFormLayout;
    auto* userEdit = new QLineEdit;
    auto* pwdEdit  = new QLineEdit;
    pwdEdit->setEchoMode(QLineEdit::Password);
    form->addRow(QString::fromStdString(wb::i18n::str_in("login", "username")), userEdit);
    form->addRow(QString::fromStdString(wb::i18n::str_in("login", "password")),   pwdEdit);
    lay->addLayout(form);
    be.bind_text(vm.username, view_for(userEdit));
    be.bind_text(vm.password, view_for(pwdEdit));
    auto* btn = new QPushButton(QString::fromStdString(wb::i18n::str_in("login", "login")));
    lay->addWidget(btn);
    auto* spinner = new QProgressBar;
    spinner->setRange(0, 0);
    spinner->setVisible(false);
    lay->addWidget(spinner);
    auto* errLbl = new QLabel;
    errLbl->setStyleSheet("QLabel { color:#b71c1c; font-weight:bold; }");
    lay->addWidget(errLbl);
    auto* welcome = new QLabel;
    lay->addWidget(welcome);
    auto* activeLbl = new QLabel;
    lay->addWidget(activeLbl);
    lay->addStretch();
    QObject::connect(btn, &QPushButton::clicked, [&vm] { vm.submit(); });
    // Button enabled/text mixes TWO properties (is_active && !is_executing)
    // plus a label swap — that is genuinely a view-level composition of two
    // VM states, so it stays hand-wired. (If it grew, the right home would
    // be a `Computed<bool>` on the VM, not a binding helper.)
    auto refreshBtn = [btn, &vm] {
        const bool busy   = vm.login.is_executing.get();
        const bool active = vm.is_active().get();
        btn->setEnabled(active && !busy);
        btn->setText(busy ? QString::fromStdString(wb::i18n::str_in("login", "logging_in")) : QString::fromStdString(wb::i18n::str_in("login", "login")));
    };
    refreshBtn();
    s_subs.push_back(vm.login.is_executing.on_changed([refreshBtn](bool){ refreshBtn(); }));
    // Active/inactive status label — composed from i18n keys + state glyph.
    auto syncActive = [activeLbl, &vm](bool a) {
        const std::string key = a ? "vm_active" : "vm_inactive";
        activeLbl->setText(QString::fromStdString(wb::i18n::str_in("login", key))
                           + (a ? " ●" : " ○"));
    };
    syncActive(vm.is_active().get());
    s_subs.push_back(vm.is_active().on_changed([syncActive, refreshBtn](bool a) {
        refreshBtn();
        syncActive(a);
    }));
    // Single-property VM→View projections — now one-liners via the new
    // async-agnostic binders instead of hand-written on_changed subscriptions.
    be.bind_visible(vm.login.is_executing, view_for(spinner));
    be.bind_text_projected(vm.login.last_error_message, view_for(errLbl),
        [](const std::string& e) { return e.empty() ? std::string{} : "✗ " + e; });
    be.bind_optional_text(vm.login.last_result, view_for(welcome),
        [](const LoginResult& r) { return r.welcome; },
        wb::i18n::str_in("login", "not_logged_in"));
    return w;
}
}  // namespace wb::login::qtview

namespace wb::login {
void register_login_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "login",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<LoginVm&>(vm), be);
        });
}
}  // namespace wb::login
