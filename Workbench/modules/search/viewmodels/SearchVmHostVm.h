#pragma once
//
// SearchVmHostVm — adapts the plain (non-ViewModel) SearchVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
#include "module_api/BaseVm.h"
#include "viewmodels/SearchVm.h"

namespace wb::search {

class SearchVmHostVm final : public wb::core::BaseVm {
public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    aria::Property<std::string> placeholder;
    aria::Property<std::string> searchesLabel;
    explicit SearchVmHostVm(aria::IDelayedScheduler& timer) : vm_(timer) {
        text(title, "title");
        text(desc, "desc");
        text(placeholder, "placeholder");
        text(searchesLabel, "searches");
    }
    SearchVm& inner() { return vm_; }
    const SearchVm& inner() const { return vm_; }
private:
    SearchVm vm_;
};

}  // namespace wb::search
