#pragma once
//
// ThemeVmHostVm — adapts the plain (non-ViewModel) ThemeVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
// Also relocalizes the inner ThemeVm's currentDisplayName on language
// change so the picker label stays in sync with the selected language.
//
#include "module_api/BaseVm.h"
#include "viewmodels/ThemeVm.h"

namespace wb::theme {

class ThemeVmHostVm final : public wb::core::BaseVm {
public:
    ThemeVmHostVm() {
        text(title, "title");
        text(desc,  "desc");
        // On construction seed the display name for the default theme.
        vm_.currentDisplayName.set(vm_.display_name(vm_.currentId.get()));
        // Re-resolve the display name whenever the UI language changes.
        localize([this] {
            vm_.currentDisplayName.set(vm_.display_name(vm_.currentId.get()));
        });
    }

    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    ThemeVm& inner() { return vm_; }
    const ThemeVm& inner() const { return vm_; }
private:
    ThemeVm vm_;
};

}  // namespace wb::theme
