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
    }

    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    WizardVm& inner() { return vm_; }
    const WizardVm& inner() const { return vm_; }
private:
    WizardVm vm_;
};

}  // namespace wb::wizard
