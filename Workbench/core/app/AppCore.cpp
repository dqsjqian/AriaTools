#include "app/AppCore.h"
#include "app/ModulesManifest.h"

namespace wb::core {

AppCore::AppCore(std::string i18nBaseDir, std::string initialLang)
    : hub_(std::move(i18nBaseDir), std::move(initialLang)),
      ctx_(hub_),
      navigator_(std::make_shared<wb::module_api::NavigatorHost>(ctx_)),
      mounts_(std::make_shared<wb::module_api::MountRegistry>(ctx_)),
      pages_(ctx_)
{
    // Cross-module navigation: business VMs resolve other modules' VMs
    // through ModuleContext. The factory creates fresh instances so pushed
    // pages are independent of the main-tab module VMs.
    ctx_.set_vm_factory(
        [this](const std::string& id) { return create_module_vm(id); });
    // Primary-VM resolver for extension points: providers mount the module's
    // main-tab instance so mounted UI shares state with the tab (Android
    // side-channel command routing then needs no mount-aware dispatch).
    ctx_.set_primary_vm([this](const std::string& id) { return pages_.get(id); });
    ctx_.set_navigator(navigator_);
    ctx_.set_mounts(mounts_);

    // load_modules() is called by the platform shell AFTER set_ui_executor /
    // set_timer. Doing it here would trip AsyncCommand's graph thread-affinity
    // check on any module that creates an AsyncCommand (e.g. login).
}

std::shared_ptr<aria::binding::ViewModel>
AppCore::create_module_vm(const std::string& moduleId) {
    return pages_.create(moduleId);
}

void AppCore::load_modules() {
    lifecycle_.start({
        [this] {
            try { wb::app::populate_modules(registry_); }
            catch (...) { registry_.clear(); throw; }
            return aria::Subscription([this] { registry_.clear(); });
        },
        [this] {
            try {
                for (const auto& module : registry_.ordered()) {
                    page_registrations_ += pages_.add(module->id(),
                        [module](wb::module_api::ModuleContext& context) {
                            return module->create_view_model(context);
                        });
                }
            } catch (...) { page_registrations_.clear(); throw; }
            return aria::Subscription([this] {
                page_registrations_.clear();
                pages_.clear_cache();
            });
        },
        [this] {
            try {
                for (const auto& module : registry_.ordered()) module->register_navigation(*navigator_);
                for (const auto& module : registry_.ordered()) module->register_mounts(*mounts_);
            } catch (...) { navigator_->clear(); mounts_->clear(); throw; }
            return aria::Subscription([this] { navigator_->clear(); mounts_->clear(); });
        },
        [this] {
            try {
                for (const auto& module : registry_.ordered()) {
                    entries_.push_back({module->id(), module->nav_key(), pages_.get(module->id())});
                }
            } catch (...) { entries_.clear(); pages_.clear_cache(); throw; }
            return aria::Subscription([this] { entries_.clear(); });
        },
    });
}

AppCore::~AppCore() { lifecycle_.stop(); }

std::string AppCore::nav_title(const std::string& navKey) {
    return hub_.i18n().tr("common", navKey);
}

}  // namespace wb::core
