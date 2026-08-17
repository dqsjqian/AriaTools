#include "app/AppCore.h"
#include "app/ModulesManifest.h"

namespace wb::core {

AppCore::AppCore(std::string i18nBaseDir, std::string initialLang)
    : hub_(std::move(i18nBaseDir), std::move(initialLang)),
      ctx_(hub_),
      navigator_(std::make_shared<wb::module_api::NavigatorHost>(ctx_)),
      mounts_(std::make_shared<wb::module_api::MountRegistry>(ctx_))
{
    // Cross-module navigation: business VMs resolve other modules' VMs
    // through ModuleContext. The factory creates fresh instances so pushed
    // pages are independent of the main-tab module VMs.
    ctx_.set_vm_factory(
        [this](const std::string& id) { return create_module_vm(id); });
    ctx_.set_navigator(navigator_);
    ctx_.set_mounts(mounts_);

    // load_modules() is called by the platform shell AFTER set_ui_executor /
    // set_timer. Doing it here would trip AsyncCommand's graph thread-affinity
    // check on any module that creates an AsyncCommand (e.g. login).
}

std::shared_ptr<aria::binding::ViewModel>
AppCore::create_module_vm(const std::string& moduleId) {
    for (const auto& m : registry_.all()) {
        if (m->id() == moduleId) {
            return m->create_view_model(ctx_);
        }
    }
    return nullptr;
}

void AppCore::load_modules() {
    if (loaded_) return;
    loaded_ = true;
    wb::app::populate_modules(registry_);

    // Let each module register its cross-module navigation targets
    // (interface → page factory) before any VM is created.
    for (const auto& m : registry_.ordered()) {
        m->register_navigation(*navigator_);
    }

    // Then let each module provide cross-module extension points
    // (slot → UI factory) — hosts resolve slots when they render.
    for (const auto& m : registry_.ordered()) {
        m->register_mounts(*mounts_);
    }

    for (const auto& m : registry_.ordered()) {
        entries_.push_back(ModuleEntry{
            m->id(),
            m->nav_key(),
            m->create_view_model(ctx_),
        });
    }
}

AppCore::~AppCore() = default;

std::string AppCore::nav_title(const std::string& navKey) {
    return hub_.i18n().tr("common", navKey);
}

}  // namespace wb::core
