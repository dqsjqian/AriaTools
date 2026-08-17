#pragma once
//
// SignupVmHostVm — adapts the plain (non-ViewModel) SignupVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
#include "module_api/BaseVm.h"
#include "viewmodels/SignupVm.h"

namespace wb::signup {

class SignupVmHostVm final : public wb::core::BaseVm {
public:
    SignupVmHostVm() {
        text(title, "title");
        text(desc,  "desc");
    }

    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    SignupVm& inner() { return vm_; }
    const SignupVm& inner() const { return vm_; }
private:
    SignupVm vm_;
};

}  // namespace wb::signup
