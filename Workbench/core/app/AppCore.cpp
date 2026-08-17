#include "app/AppCore.h"
#include "app/ModulesManifest.h"

namespace wb::core {

AppCore::AppCore(std::string i18nBaseDir, std::string initialLang)
    : hub_(std::move(i18nBaseDir), std::move(initialLang)),
      ctx_(hub_)
{
    // load_modules() is called by the platform shell AFTER set_ui_executor /
    // set_timer. Doing it here would trip AsyncCommand's graph thread-affinity
    // check on any module that creates an AsyncCommand (e.g. login).
}

void AppCore::load_modules() {
    if (loaded_) return;
    loaded_ = true;
    wb::app::populate_modules(registry_);

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
