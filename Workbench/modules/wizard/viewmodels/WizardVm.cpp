#include "viewmodels/WizardVm.h"

#include "infra/i18n/I18n.h"

#include <utility>

namespace wb::wizard {

namespace {

/// Map the wizard's internal theme id to its localized display name.
/// The wizard keeps its own small theme vocabulary (independent of the
/// theme module) so the demo stays self-contained.
const char* theme_i18n_key(const std::string& id) {
    if (id == "Light")     return "theme_light";
    if (id == "Dark")      return "theme_dark";
    if (id == "Solarized") return "theme_solarized";
    return "theme_light";
}

}  // namespace

Step1Vm::Step1Vm(std::shared_ptr<WizardDraft> d) : draft(std::move(d)) {}
Step2Vm::Step2Vm(std::shared_ptr<WizardDraft> d) : draft(std::move(d)) {}
Step3Vm::Step3Vm(std::shared_ptr<WizardDraft> d) : draft(std::move(d)) {}

void Step3Vm::finish() {
    // Compose the finished summary from current draft values, resolving
    // the theme's display name via i18n at click time so language
    // switches are reflected immediately.
    finishedSummary.set(
        "✓ " + draft->username.get() + " (" +
        draft->email.get() + ", " +
        wb::i18n::str_in("wizard", theme_i18n_key(draft->theme.get())) + ") " +
        wb::i18n::str_in("wizard", "registered_suffix"));
}

WizardVm::WizardVm()
    : draft(std::make_shared<WizardDraft>()),
      nav(std::make_shared<aria::binding::Navigator>())
{
    step1 = std::make_shared<Step1Vm>(draft);
    step2 = std::make_shared<Step2Vm>(draft);
    step3 = std::make_shared<Step3Vm>(draft);
    nav->push(step1);
}

void WizardVm::toStep(int i) {
    auto target = (i == 1) ? std::static_pointer_cast<aria::binding::ViewModel>(step1)
                : (i == 2) ? std::static_pointer_cast<aria::binding::ViewModel>(step2)
                           : std::static_pointer_cast<aria::binding::ViewModel>(step3);
    nav->replace(target);
}

}  // namespace wb::wizard
