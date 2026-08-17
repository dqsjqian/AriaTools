#pragma once
//
// AppCore — Platform-independent application core. Assembles the ServiceHub
// (stable services) and walks the module registry to build VMs.
// Contains no UI adapters or BindingEngine (each platform shell holds those).
//
#include "infra/ServiceHub.h"
#include "module_api/ModuleRegistry.h"
#include "module_api/ModuleContext.h"
#include "module_api/MountRegistry.h"
#include "module_api/NavigatorHost.h"

#include <memory>
#include <string>
#include <vector>

namespace wb::core {

/// An assembled module entry (id + navigation title key + VM instance).
struct ModuleEntry {
    std::string id;
    std::string navKey;
    std::shared_ptr<aria::binding::ViewModel> vm;
};

class AppCore {
public:
    /// i18nBaseDir: runtime i18n/ directory path (a <mod>/ subdirectory per module's text).
    /// Construction only initializes the ServiceHub. Call set_ui_executor /
    /// set_timer to inject platform services BEFORE load_modules(), otherwise
    /// AsyncCommand-bearing VMs (e.g. login) will throw at construction time.
    explicit AppCore(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~AppCore();

    AppCore(const AppCore&) = delete;
    AppCore& operator=(const AppCore&) = delete;

    /// Inject platform executors (must be called BEFORE load_modules()).
    /// QtAppShell sets QtDispatcher (which doubles as IExecutor +
    /// IDelayedScheduler) so AsyncCommand gets a UI-thread co_await target
    /// that satisfies the graph thread-affinity invariant (InlineExecutor
    /// fallback would trip it when worker runs on a different thread).
    void set_ui_executor(aria::async::IExecutor* e) { hub_.set_ui_executor(e); }
    void set_timer(aria::IDelayedScheduler* t)     { hub_.set_timer(t); }

    /// Walk the module registry, create each VM, and populate entries_.
    /// Idempotent: calling twice is a no-op.
    void load_modules();

    [[nodiscard]] const std::vector<ModuleEntry>& modules() const { return entries_; }
    [[nodiscard]] wb::infra::ServiceHub& services() { return hub_; }
    [[nodiscard]] wb::services::II18nService& i18n() { return hub_.i18n(); }

    /// Get a module's navigation title (current language, from the common module's nav_ key).
    [[nodiscard]] std::string nav_title(const std::string& navKey);

    /// Create a *fresh* ViewModel instance of `moduleId` (cross-module
    /// navigation). Each call builds a new instance via the module factory,
    /// so pushed pages never share state with the main-tab module VM.
    /// Returns nullptr if the module id is unknown.
    [[nodiscard]] std::shared_ptr<aria::binding::ViewModel>
    create_module_vm(const std::string& moduleId);

    /// Cross-module navigator (owns the module→page registry; modules
    /// registered their targets via IModule::register_navigation during
    /// load_modules).
    [[nodiscard]] wb::module_api::NavigatorHost& navigator() { return *navigator_; }

    /// Cross-module mount registry (owns the slot→provider map; modules
    /// provided slots via IModule::register_mounts during load_modules).
    [[nodiscard]] wb::module_api::MountRegistry& mounts() { return *mounts_; }

private:
    wb::infra::ServiceHub                     hub_;
    wb::module_api::ModuleRegistry            registry_;
    wb::module_api::ModuleContext             ctx_;
    std::shared_ptr<wb::module_api::NavigatorHost> navigator_;
    std::shared_ptr<wb::module_api::MountRegistry> mounts_;
    std::vector<ModuleEntry>                  entries_;
    bool                                      loaded_ = false;
};

}  // namespace wb::core
