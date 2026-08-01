#pragma once
//
// ISettingsService — Read/write application settings (including sync settings).
// Skeleton stage: stub implementation stores in memory / local config/settings.json.
//
#include "infra/settings/SyncTypes.h"
#include <string>

namespace wb::services {

class ISettingsService {
public:
    virtual ~ISettingsService() = default;

    /// Load settings (on app startup).
    virtual void load() = 0;
    /// Persist settings (writes config/settings.json).
    virtual void save() = 0;

    [[nodiscard]] virtual SyncSettings& sync() = 0;
    [[nodiscard]] virtual const SyncSettings& sync() const = 0;

    // Theme / language and other common items (skeleton stage: two placeholders).
    [[nodiscard]] virtual std::string theme() const = 0;
    virtual void set_theme(std::string t) = 0;
    [[nodiscard]] virtual std::string language() const = 0;
    virtual void set_language(std::string l) = 0;
};

}  // namespace wb::services
