#pragma once
//
// AppCore — Platform-independent application core. Assembles the ServiceHub
// (stable services) and walks the module registry to build VMs.
// Contains no UI adapters or BindingEngine (each platform shell holds those).
//
#include "infra/ServiceHub.h"
#include "module_api/ModuleRegistry.h"
#include "module_api/ModuleContext.h"

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
    explicit AppCore(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~AppCore();

    AppCore(const AppCore&) = delete;
    AppCore& operator=(const AppCore&) = delete;

    [[nodiscard]] const std::vector<ModuleEntry>& modules() const { return entries_; }
    [[nodiscard]] wb::infra::ServiceHub& services() { return hub_; }
    [[nodiscard]] wb::services::II18nService& i18n() { return hub_.i18n(); }

    /// Get a module's navigation title (current language, from the common module's nav_ key).
    [[nodiscard]] std::string nav_title(const std::string& navKey);

private:
    wb::infra::ServiceHub                     hub_;
    wb::module_api::ModuleRegistry            registry_;
    wb::module_api::ModuleContext             ctx_;
    std::vector<ModuleEntry>                  entries_;
};

}  // namespace wb::core
