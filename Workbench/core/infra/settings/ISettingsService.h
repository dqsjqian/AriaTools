#pragma once
//
// ISettingsService — 读写应用设置（含同步设置）。
// 骨架期：桩实现存内存 / 本地 config/settings.json。
//
#include "infra/settings/SyncTypes.h"
#include <string>

namespace wb::services {

class ISettingsService {
public:
    virtual ~ISettingsService() = default;

    /// 载入设置（应用启动时）。
    virtual void load() = 0;
    /// 持久化设置（写 config/settings.json）。
    virtual void save() = 0;

    [[nodiscard]] virtual SyncSettings& sync() = 0;
    [[nodiscard]] virtual const SyncSettings& sync() const = 0;

    // 主题 / 语言等通用项（骨架期先放两个占位）。
    [[nodiscard]] virtual std::string theme() const = 0;
    virtual void set_theme(std::string t) = 0;
    [[nodiscard]] virtual std::string language() const = 0;
    virtual void set_language(std::string l) = 0;
};

}  // namespace wb::services
