#pragma once
//
// MountRegistry — cross-module extension points ("slots").
//
// A HOST module declares a slot id in its View layout; a PROVIDER module
// registers a factory for that slot; the host's View resolves it by id.
// Both sides stay fully decoupled — the host never includes the provider's
// headers, the provider never knows which host consumes it. Same philosophy
// as NavigatorHost, but for "where UI is mounted" instead of "which page
// to push" (VS Code contributes.views / Eclipse extension-point style).
//
// The resolved result carries the provider's moduleId so the platform View
// factory (QtViewFactory / UIViewFactory / ComposeViewFactory) can build the
// view. If no provider registers (module removed / never installed),
// Resolve returns nullopt and the host renders a placeholder — graceful
// degradation identical to NavigatorHost::Push returning false.
//
// Two example flows:
//   provider: mounts.Provide("dashboard.content", "cart", factory);
//   host:     if (auto m = mounts.Resolve("dashboard.content"))
//                 render(m->moduleId, *m->vm);   // view factory by id
//             else
//                 render_placeholder();
//
// Mounts are independent of the navigation stack: pushing a page is
// "navigate", providing a slot is "extend". Unprovide() lets a host toggle
// an extension live (demonstrated by dashboard's mount toggle).
//

#include "aria/binding/view_model.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace wb::module_api {

// Forward declaration only: ModuleContext.h includes this header (for the
// mounts() accessor), so we must NOT include it back here — MountRegistry
// only holds a reference and never needs ModuleContext's definition.
class ModuleContext;

/// A resolved mount: the provider's module id + a fresh ViewModel instance.
struct Mounted {
    std::string moduleId;
    std::shared_ptr<aria::binding::ViewModel> vm;
};

/// Well-known demo slot ids. Kept here (framework kernel) so both host and
/// provider agree on the key without including each other — same idea as
/// the kParam* constants in ICartPage.
namespace slots {
inline constexpr const char* kDashboardContent = "dashboard.content";
}

class MountRegistry {
public:
    explicit MountRegistry(ModuleContext& ctx) : ctx_(ctx) {}

    using Factory =
        std::function<std::shared_ptr<aria::binding::ViewModel>(ModuleContext&)>;

    /// Provider module registers a factory for `slotId`. Returns false if
    /// the slot is already provided (no silent overwrite).
    bool Provide(std::string slotId, std::string moduleId, Factory factory) {
        auto [it, inserted] = slots_.try_emplace(
            std::move(slotId), Entry{std::move(moduleId), std::move(factory), true});
        return inserted;
    }

    /// Host resolves the slot: builds a fresh VM via the provider's factory
    /// (each resolution a fresh instance, like NavigatorHost's per-push VMs).
    /// Returns nullopt when the slot has no enabled provider (graceful
    /// degradation) — the host renders a placeholder instead.
    [[nodiscard]] std::optional<Mounted> Resolve(const std::string& slotId) {
        auto it = slots_.find(slotId);
        if (it == slots_.end() || !it->second.enabled) return std::nullopt;
        auto vm = it->second.factory(ctx_);
        if (!vm) return std::nullopt;
        return Mounted{it->second.moduleId, std::move(vm)};
    }

    /// The provider's module id for a slot ("" / nullopt when unprovided or
    /// disabled). Lets hosts show "mounted: <module>" without resolving a VM.
    [[nodiscard]] std::optional<std::string> module_of(const std::string& slotId) const {
        auto it = slots_.find(slotId);
        if (it == slots_.end() || !it->second.enabled) return std::nullopt;
        return it->second.moduleId;
    }

    /// Temporarily enable/disable a slot WITHOUT losing its provider factory.
    /// Lets a host toggle an extension live without knowing the provider —
    /// the provider registered once, the host just flips the switch.
    /// Returns false if the slot is unknown.
    bool SetEnabled(const std::string& slotId, bool enabled) {
        auto it = slots_.find(slotId);
        if (it == slots_.end()) return false;
        it->second.enabled = enabled;
        return true;
    }

    /// Remove the provider for a slot entirely (module unload). Returns
    /// false if the slot was not provided.
    bool Unprovide(const std::string& slotId) {
        return slots_.erase(slotId) > 0;
    }

    /// True if the slot currently has an enabled provider.
    [[nodiscard]] bool provided(const std::string& slotId) const {
        auto it = slots_.find(slotId);
        return it != slots_.end() && it->second.enabled;
    }

private:
    struct Entry {
        std::string moduleId;
        Factory factory;
        bool enabled = true;
    };

    ModuleContext& ctx_;
    std::unordered_map<std::string, Entry> slots_;
};

}  // namespace wb::module_api
