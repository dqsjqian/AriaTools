#pragma once
//
// NavigationEntryVm — a navigation-stack entry that carries the target
// module's identity. Lets an Aria `Navigator` push *any* module's
// ViewModel as a page, not just child VMs of the current module.
//
//   wb::module_api::NavigationEntryVm entry{"cart", cartVmInstance};
//   nav->push(entry);   // Navigator activate/deactivate semantics apply
//
// The View layer reads `module_id()` to pick the right ViewFactory builder
// and `inner()` to bind against; the routing decision itself stays in the
// ViewModel layer (see AppCore::create_module_vm + ModuleContext).
//

#include "aria/binding/view_model.hpp"

#include <memory>
#include <string>

namespace wb::module_api {

class NavigationEntryVm final : public aria::binding::ViewModel {
public:
    NavigationEntryVm(std::string moduleId,
                      std::shared_ptr<aria::binding::ViewModel> inner)
        : moduleId_(std::move(moduleId)), inner_(std::move(inner)) {}

    /// Target module id; the View uses it to resolve the ViewFactory builder.
    [[nodiscard]] const std::string& module_id() const noexcept {
        return moduleId_;
    }

    /// The actual target ViewModel (owned by this entry).
    [[nodiscard]] aria::binding::ViewModel& inner() { return *inner_; }
    [[nodiscard]] const aria::binding::ViewModel& inner() const { return *inner_; }

    void on_activate() override {
        if (inner_) inner_->activate();
    }

    void on_deactivate() override {
        if (inner_) inner_->deactivate();
    }

private:
    std::string moduleId_;
    std::shared_ptr<aria::binding::ViewModel> inner_;
};

}  // namespace wb::module_api
