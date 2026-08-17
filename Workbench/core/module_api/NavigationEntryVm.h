#pragma once
//
// NavigationEntryVm — a navigation-stack entry that carries the target
// module's identity AND how it should be presented. Lets an Aria
// `Navigator` push *any* module's ViewModel as a page, not just child
// VMs of the current module.
//
//   wb::module_api::NavigationEntryVm entry{"cart", cartVmInstance,
//                                           Presentation::Modal};
//   nav->push(entry);   // Navigator activate/deactivate semantics apply
//
// The View layer reads `module_id()` to pick the right ViewFactory builder,
// `inner()` to bind against, and `presentation()` to decide HOW to render
// it (embedded page / modal dialog / standalone window). The routing
// decision itself stays in the ViewModel layer (see AppCore::create_module_vm
// + ModuleContext).
//
// Presentation is the industry-standard "how does this route appear"
// concept (React Navigation `presentation`, Vue Router `meta`, Flutter
// PopupRoute). A caller picks one per navigation via NavOptions.
//

#include "aria/binding/view_model.hpp"

#include <memory>
#include <string>

namespace wb::module_api {

/// How a navigation target should be presented to the user. Maps onto the
/// industry-standard "route presentation" concept; each platform View shell
/// renders a kind with its native equivalent:
///   Push   -> embedded page container (Qt QStackedWidget / iOS child VC / Compose)
///   Modal  -> modal overlay/dialog (Qt QDialog / iOS present VC / Compose Dialog)
///   Window -> standalone top-level window (Qt new QWidget window; mobile
///             has no separate-window concept and falls back to Modal).
enum class Presentation : unsigned char {
    Push   = 0,
    Modal  = 1,
    Window = 2,
};

/// Options for a NavigatorHost::Push call. Kept as a struct so future
/// per-navigation knobs (animation, clear-stack, deep-link) extend without
/// signature churn — same pattern as React Navigation `options` / Vue `meta`.
struct NavOptions {
    Presentation presentation{Presentation::Push};
};

class NavigationEntryVm final : public aria::binding::ViewModel {
public:
    NavigationEntryVm(std::string moduleId,
                      std::shared_ptr<aria::binding::ViewModel> inner,
                      Presentation pres = Presentation::Push)
        : moduleId_(std::move(moduleId)),
          inner_(std::move(inner)),
          presentation_(pres) {}

    /// Target module id; the View uses it to resolve the ViewFactory builder.
    [[nodiscard]] const std::string& module_id() const noexcept {
        return moduleId_;
    }

    /// How this entry should be presented (embedded / modal / window).
    [[nodiscard]] Presentation presentation() const noexcept {
        return presentation_;
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
    Presentation presentation_{Presentation::Push};
};

}  // namespace wb::module_api
