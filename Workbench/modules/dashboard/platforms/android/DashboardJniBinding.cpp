// ────────────────────────────────────────────────────────────────────────────
//  DashboardJniBinding.cpp — see DashboardJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/DashboardJniBinding.h"

namespace wb::dashboard {

void subscribe_dashboard(aria::runtime::EventBus& bus, DashboardVm& vm,
                         std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::push_property;

    bind_str(subs, "dashboard", "welcome", vm.welcome);
    bind_str(subs, "dashboard", "summary", vm.summary);
    bind_str(subs, "dashboard", "cartBadge", vm.cartBadge);
    bind_str(subs, "dashboard", "lastOrder", vm.lastOrder);
    // Navigation demo labels.
    bind_str(subs, "dashboard", "modal_cart",  vm.modalCartLabel);
    bind_str(subs, "dashboard", "window_cart", vm.windowCartLabel);
    bind_str(subs, "dashboard", "back",        vm.navBackLabel);
    // Extension point (mount) labels + state.
    bind_str(subs, "dashboard", "mount_toggle",  vm.mountToggleLabel);
    bind_str(subs, "dashboard", "mount_status",  vm.mountStatus);
    bind_str(subs, "dashboard", "mountedModule", vm.mountedModule);
    // Navigation mirrors (pushed over the side-channel so Compose can
    // render the pushed page by module id + presentation kind).
    bind_str(subs, "dashboard", "navCurrentModule", vm.navCurrentModule);
    wb::jni::bind_int(subs, "dashboard", "navDepth", vm.navDepth);
    wb::jni::bind_int(subs, "dashboard", "navPresentation", vm.navPresentation);
    push_property("dashboard", "navDepth", std::to_string(vm.navDepth.get()));
    push_property("dashboard", "navPresentation",
                  std::to_string(vm.navPresentation.get()));
}

void set_dashboard_text(DashboardVm& vm, const std::string& propName,
                        const std::string& value) {
    (void)vm; (void)propName; (void)value;
    // dashboard has no user-editable string properties.
}

void exec_dashboard_command(DashboardVm& vm, const std::string& cmdName) {
    if (cmdName == "modalCart") vm.modalCart.execute();
    else if (cmdName == "windowCart") vm.windowCart.execute();
    else if (cmdName == "navBack") vm.navBack.execute();
    else if (cmdName == "mountToggle") vm.mountToggle.execute();
}

void register_dashboard_binding(wb::jni::BindingTable& table) {
    table["dashboard"] = wb::jni::ModuleBinding{
        [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
           std::vector<aria::Subscription>& subs) {
            subscribe_dashboard(bus, static_cast<DashboardVm&>(vm), subs);
        },
        [](aria::binding::ViewModel& vm, const std::string& prop,
           const std::string& value) {
            set_dashboard_text(static_cast<DashboardVm&>(vm), prop, value);
        },
        [](aria::binding::ViewModel& vm, const std::string& cmd) {
            exec_dashboard_command(static_cast<DashboardVm&>(vm), cmd);
        }};
}

}  // namespace wb::dashboard
