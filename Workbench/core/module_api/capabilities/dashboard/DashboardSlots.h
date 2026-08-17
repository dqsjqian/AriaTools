#pragma once
//
// DashboardSlots — cross-module extension-point contract for the dashboard
// module: the slot ids the dashboard declares for hosts to fill.
//
// Lives in module_api (capabilities layer, like ICartPage) so any provider
// module can reference the dashboard's slots by including only this
// framework header — zero coupling: the dashboard declares the slot, the
// provider fills it, neither includes the other's headers. If the dashboard
// module is removed, providers simply never get resolved (slots go idle).
//
// Framework kernel (MountRegistry.h) intentionally does NOT know business
// slot ids — it only provides the Provide/Resolve mechanism.
//

namespace wb::module_api {

/// Extension-point slots declared by the "dashboard" module.
struct DashboardSlots {
    /// Main content area below the dashboard's buttons. The cart module
    /// provides this slot (register_mounts) and the dashboard resolves it
    /// when rendering its host area.
    static constexpr const char* kContent = "dashboard.content";
};

}  // namespace wb::module_api
