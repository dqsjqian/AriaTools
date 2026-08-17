#pragma once
//
// WizardVmHostVm — adapts the plain (non-ViewModel) WizardVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
#include "module_api/BaseVm.h"
#include "viewmodels/WizardVm.h"

namespace wb::wizard {

class WizardVmHostVm final : public wb::core::BaseVm {
public:
    WizardVmHostVm() {
        text(title, "title");
        text(desc,  "desc");
        text(step1Label,       "step1");
        text(step2Label,       "step2");
        text(step3Label,       "step3");
        text(usernameLabel,    "username");
        text(emailLabel,        "email");
        text(finishLabel,       "finish");
        text(unfinishedLabel,   "unfinished");
        text(themeLightLabel,     "theme_light");
        text(themeDarkLabel,      "theme_dark");
        text(themeSolarizedLabel, "theme_solarized");
    }

    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    aria::Property<std::string> step1Label;
    aria::Property<std::string> step2Label;
    aria::Property<std::string> step3Label;
    aria::Property<std::string> usernameLabel;
    aria::Property<std::string> emailLabel;
    aria::Property<std::string> finishLabel;
    aria::Property<std::string> unfinishedLabel;
    aria::Property<std::string> themeLightLabel;
    aria::Property<std::string> themeDarkLabel;
    aria::Property<std::string> themeSolarizedLabel;
    WizardVm& inner() { return vm_; }
    const WizardVm& inner() const { return vm_; }
private:
    WizardVm vm_;
};

}  // namespace wb::wizard
