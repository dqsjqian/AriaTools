#include "SignupView.h"
#include "support/UiHelpers.h"
#include "viewmodels/SignupVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::signup::qtview {
using namespace wb::ui;
namespace {
// Per-field error label rendering.
//
// FormField exposes:
//   - is_valid : Property<bool>
//   - error    : Property<std::string>   (single message, empty when valid)
// We render "✓" when valid and "✗ <error>" when not. This matches the
// new FormField shape — there is no longer a ValidationResult vector.
void render_field_hint(QLabel* out, bool valid, const std::string& err) {
    if (valid) {
        out->setText("✓");
        out->setStyleSheet("QLabel { color:#1b5e20; font-size:11px; }");
        return;
    }
    out->setText("✗ " + QString::fromStdString(err));
    out->setStyleSheet("QLabel { color:#b71c1c; font-size:11px; }");
}
void wire_field(std::vector<aria::Subscription>& subs,
                aria::binding::BindingEngine& be,
                aria::binding::FormField<std::string>& field,
                QLineEdit* edit,
                QLabel* hint) {
    be.bind_text(field.value, view_for(edit));
    auto sync = [hint, &field] {
        render_field_hint(hint, field.is_valid.get(), field.error.get());
    };
    sync();
    subs.push_back(field.is_valid.on_changed([sync](bool) { sync(); }));
    subs.push_back(field.error   .on_changed([sync](const std::string&) { sync(); }));
}
}  // namespace

SignupView::SignupView(SignupVm& host, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& vm = host;
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(host.desc, view_for(info));

    auto* form = new QFormLayout;
    auto* userEdit  = new QLineEdit; auto* userHint  = new QLabel;
    auto* emailEdit = new QLineEdit; auto* emailHint = new QLabel;
    auto* pwdEdit   = new QLineEdit; pwdEdit->setEchoMode(QLineEdit::Password);
    auto* pwdHint   = new QLabel;
    auto* confEdit  = new QLineEdit; confEdit->setEchoMode(QLineEdit::Password);
    auto* confHint  = new QLabel;
    form->addRow(QString::fromStdString(wb::i18n::str_in("signup", "username")), userEdit);  form->addRow("", userHint);
    form->addRow(QString::fromStdString(wb::i18n::str_in("signup", "email")),   emailEdit); form->addRow("", emailHint);
    form->addRow(QString::fromStdString(wb::i18n::str_in("signup", "password")),   pwdEdit);   form->addRow("", pwdHint);
    form->addRow(QString::fromStdString(wb::i18n::str_in("signup", "confirm")),   confEdit);  form->addRow("", confHint);
    lay->addLayout(form);
    wire_field(s_subs, be, vm.username, userEdit,  userHint);
    wire_field(s_subs, be, vm.email,    emailEdit, emailHint);
    wire_field(s_subs, be, vm.password, pwdEdit,   pwdHint);
    wire_field(s_subs, be, vm.confirm,  confEdit,  confHint);
    // Form-level error banner: surfaces cross-field rule failures
    // (e.g. "passwords do not match") that have no single-field owner.
    auto* formError = new QLabel;
    formError->setStyleSheet("QLabel { color:#b71c1c; font-size:11px; }");
    formError->setWordWrap(true);
    lay->addWidget(formError);
    auto syncFormError = [formError](const std::string& msg) {
        formError->setText(msg.empty() ? QString{} : QString::fromStdString("⚠ " + msg));
        formError->setVisible(!msg.empty());
    };
    syncFormError(vm.form.first_error.get());
    s_subs.push_back(vm.form.first_error.on_changed(syncFormError));
    auto* submit = new QPushButton(QString::fromStdString(wb::i18n::str_in("signup", "submit")));
    submit->setStyleSheet(
        "QPushButton:enabled  { background:#2e7d32; color:white; padding:8px; }"
        "QPushButton:disabled { background:#bdbdbd; color:#616161; padding:8px; }");
    lay->addWidget(submit);
    be.bind_command(vm.submit, view_for(submit));
    // Submit's enabled state is now driven directly by form.is_valid —
    // the form aggregates per-field validators AND cross-field rules,
    // so this single Property is the canonical "ready to submit" gate.
    auto syncEnabled = [submit](bool ok) { submit->setEnabled(ok); };
    syncEnabled(vm.form.is_valid.get());
    s_subs.push_back(vm.form.is_valid.on_changed(syncEnabled));
    auto* summary = new QLabel;
    summary->setText(QString::fromStdString(wb::i18n::str_in("signup", "unregistered")));
    lay->addWidget(summary);
    be.bind_text_oneway(vm.submittedSummary, view_for(summary));
    lay->addStretch();
}

}  // namespace wb::signup::qtview
