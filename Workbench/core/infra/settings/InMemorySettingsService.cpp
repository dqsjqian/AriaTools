//
// InMemorySettingsService — 骨架期设置桩实现。
// 默认 dataDir = ~/WorkbenchData。save()/load() 仅打日志占位，
// 业务阶段替换为 config/settings.json 读写。
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

    void load() override { /* 骨架期：无持久化 */ }
    void save() override { /* 骨架期：无持久化 */ }

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

// 工厂（供装配根使用，避免暴露具体类型）。
ISettingsService* make_in_memory_settings_service() {
    return new InMemorySettingsService();
}

}  // namespace wb::services
