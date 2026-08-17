#include "viewmodels/SignupVm.h"

#include "infra/i18n/I18n.h"

#include <cctype>
#include <optional>

namespace wb::signup {

namespace {

/// Build a rule body whose returned message is resolved against the
/// current UI language at *evaluate* time, so language switches are
/// reflected immediately without re-wiring the Validator.
template<std::predicate<const std::string&> P>
aria::Validator<std::string>::Rule localized_rule(P pred, const char* key) {
    return [pred = std::move(pred), key](const std::string& v)
               -> std::optional<std::string> {
        if (pred(v)) return std::nullopt;
        return wb::i18n::str_in("signup", key);
    };
}

}  // namespace

SignupVm::SignupVm()
    : submit([this] {
          // Compose the post-submit summary from the current form values
          // and localized suffix; rebuilt on every submit so language
          // changes are reflected immediately.
          submittedSummary.set(
              "✓ " + username.value.get() +
              " (" + email.value.get() + ") " +
              wb::i18n::str_in("signup", "registered_suffix"));
      })
{
    text(title, "title");
    text(desc, "desc");

    // ── Per-field rules (messages resolved at evaluate time via
    //    localized_rule, so language switches refresh them with no
    //    rebuild) ──────────────────────────────────────────────────
    username
        .rule(localized_rule(
            [](const std::string& s) { return s.size() >= 3; },
            "err_username_short"))
        .rule(localized_rule(
            [](const std::string& s) { return s.size() <= 16; },
            "err_username_long"));

    email
        .rule(localized_rule(
            [](const std::string& s) { return !s.empty(); },
            "err_email_required"))
        .rule(localized_rule(
            [](const std::string& s) {
                return s.find('@') != std::string::npos
                    && s.find('.') != std::string::npos;
            },
            "err_email_invalid"));

    password
        .rule(localized_rule(
            [](const std::string& s) { return s.size() >= 6; },
            "err_password_short"))
        .rule(localized_rule(
            [](const std::string& s) {
                for (char c : s) {
                    if (std::isdigit(static_cast<unsigned char>(c))) return true;
                }
                return false;
            },
            "err_password_no_digit"));

    // confirm has no per-field rule of its own — the password-match
    // check is a cross-field rule on the FormValidator below.

    // Re-wire the aggregate whenever the UI language changes so the
    // cross-field message is resolved against the new language.
    localize([this] { wire_form_(); });
}

SignupVm::~SignupVm() = default;

void SignupVm::wire_form_() {
    // clear() drops previously tracked fields and rules so we do not
    // accumulate stale subscriptions across language switches.
    form.clear();

    form.track(username);
    form.track(email);
    form.track(password);
    form.track(confirm);

    form.rule(
        [this] {
            return password.value.get() == confirm.value.get();
        },
        wb::i18n::str_in("signup", "err_password_mismatch"));
}

}  // namespace wb::signup
