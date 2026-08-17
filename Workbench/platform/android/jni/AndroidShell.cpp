#include "AndroidShell.h"

namespace wb::android {

AndroidShell::AndroidShell(std::string i18nBaseDir, std::string initialLang)
    : core_(std::move(i18nBaseDir), std::move(initialLang)) {
    // Snapshot module metadata once; titles are resolved in the current
    // language (re-queried per language change by the shell owner if needed).
    for (const auto& m : core_.modules()) {
        modules_.push_back({m.id, m.navKey, core_.nav_title(m.navKey)});
    }
}

AndroidShell::~AndroidShell() {
    if (active_ && active_->is_active().get()) {
        active_->deactivate();
    }
    active_.reset();
}

void AndroidShell::activate_module(const std::string& id) {
    for (auto& m : core_.modules()) {
        if (m.id != id) {
            continue;
        }
        if (active_ && active_.get() != m.vm.get() && active_->is_active().get()) {
            active_->deactivate();
        }
        active_ = m.vm;
        if (!active_->is_active().get()) {
            active_->activate();
        }
        return;
    }
}

}  // namespace wb::android
