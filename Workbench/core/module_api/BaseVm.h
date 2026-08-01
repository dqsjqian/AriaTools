#pragma once
//
// BaseVm — Base class for all business ViewModels. Centralizes the low-level
// capabilities that are universally applicable to every business VM.
// Currently handles a single cross-cutting concern: auto-refreshing UI text
// on language change.
//
// Design principle: only extremely common, business-agnostic capabilities
// belong here (e.g. language change). Pure presentation-layer capabilities
// such as theme color changes do NOT belong in the VM — that is the View
// layer's job (each platform's native skinning), handled by a separate
// View-side mechanism/base class to avoid leaking presentation details into
// cross-platform logic.
//
// i18n usage (write freely in VMs; all auto-refresh on language change):
//
//   // Static text: one line; module is inferred automatically and updates on language switch.
//   text(title, "title");
//   text(addLabel, "add");
//
//   // Dynamic text: register a recompute closure; re-runs on language change;
//   // other trigger points may also call it manually.
//   localize([this]{
//       status.set(wb::i18n::str("count_prefix") + std::to_string(n) +
//                  wb::i18n::str("count_suffix"));
//   });
//
// Both execute once immediately to set the initial value, then the base class
// re-runs them automatically on language change. No track() needed, no module
// id needed, no central relocalize method needed.
//
#include "aria/binding/view_model.hpp"
#include "aria/property.hpp"
#include "aria/subscription.hpp"
#include "infra/i18n/I18n.h"

#include <functional>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

namespace wb::core {

class BaseVm : public aria::binding::ViewModel {
public:
    BaseVm() {
        // Language change -> re-run all localization closures.
        // Note: this subscription must live for the entire VM lifetime; it must NOT
        // go into track()/bag() — bag() is cleared during on_deactivate()'s
        // bag().clear() (e.g. when a tab is switched away), which would stop text
        // from refreshing on language change after returning. So use a standalone
        // member, released on VM destruction.
        lang_sub_ = wb::i18n::on_language_changed(
            [this](const std::string&) { relocalize_all_(); });
    }

protected:
    /// Register a localization closure: runs once immediately to set the
    /// initial value, and re-runs automatically on language change.
    /// Used for dynamic text (with variable interpolation) or any logic
    /// that must refresh on language change.
    void localize(std::function<void()> fn) {
        fn();
        localizers_.push_back(std::move(fn));
    }

    /// Convenience: bind a text Property to a key. Module is auto-inferred
    /// from the call site's source file.
    /// Equivalent to localize([&]{ prop.set(wb::i18n::str_in(module, key)); }).
    void text(aria::Property<std::string>& prop, std::string key,
              std::source_location loc = std::source_location::current()) {
        std::string module{wb::i18n::detail::module_of(loc.file_name())};
        localize([&prop, module = std::move(module), key = std::move(key)]() {
            prop.set(wb::i18n::str_in(module, key));
        });
    }

private:
    void relocalize_all_() {
        for (auto& fn : localizers_) fn();
    }

    std::vector<std::function<void()>> localizers_;
    aria::Subscription lang_sub_;  ///< Language subscription, lives with the VM (not in bag)
};

}  // namespace wb::core
