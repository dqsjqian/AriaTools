#pragma once
//
// UnitConvertVmHostVm — adapts the plain (non-ViewModel) UnitConvertVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
#include "module_api/BaseVm.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

class UnitConvertVmHostVm final : public wb::core::BaseVm {
public:
    UnitConvertVmHostVm() {
        text(title, "title");
        text(desc,  "desc");
    }

    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    UnitConvertVm& inner() { return vm_; }
    const UnitConvertVm& inner() const { return vm_; }
private:
    UnitConvertVm vm_;
};

}  // namespace wb::unitconvert
