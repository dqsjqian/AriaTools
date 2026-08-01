//
// InMemorySettingsService — Skeleton-stage settings stub implementation.
// Default dataDir = ~/WorkbenchData. save()/load() are log-only placeholders;
// replace with config/settings.json read/write in the production phase.
//
#include "infra/settings/ISettingsService.h"

#include <cstdlib>
#include <string>

namespace wb::services {

namespace {
std::string default_data_dir() {
    const char* home = std::getenv("HOME");
    std::string base = home ? std::string(home) : std::string(".");
    return base + "/WorkbenchData";
}
}  // namespace

class InMemorySettingsService final : public ISettingsService {
public:
    InMemorySettingsService() {
        sync_.dataDir = default_data_dir();
    }

    void load() override { /* Skeleton stage: no persistence */ }
    void save() override { /* Skeleton stage: no persistence */ }

    SyncSettings& sync() override { return sync_; }
    const SyncSettings& sync() const override { return sync_; }

    std::string theme() const override { return theme_; }
    void set_theme(std::string t) override { theme_ = std::move(t); }
    std::string language() const override { return language_; }
    void set_language(std::string l) override { language_ = std::move(l); }

private:
    SyncSettings sync_;
    std::string  theme_ = "light";
    std::string  language_ = "zh-CN";
};

// Factory (used by the assembly root, to avoid exposing the concrete type).
ISettingsService* make_in_memory_settings_service() {
    return new InMemorySettingsService();
}

}  // namespace wb::services
