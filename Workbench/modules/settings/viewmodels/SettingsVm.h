#pragma once
//
// SettingsVm — app-level preferences (currently: UI language; theme color etc. may be added later).
// Sync/remote repository configuration has been moved to the Sync module (configuration and sync actions belong to one feature).
//
#include "aria/aria.hpp"
#include <string>
#include <vector>
#include "module_api/BaseVm.h"

namespace wb::settings {

class SettingsVm final : public wb::core::BaseVm {
public:
    SettingsVm();

    aria::Property<std::string> language;   ///< Current language code

    // UI text (updates on language change)
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> languageLabel;
    /// Available languages (code, localized label) — display data owned by VM.
    struct LangEntry { std::string code; std::string label; };
    [[nodiscard]] std::vector<LangEntry> available_languages() const;

    // Language-switch button labels (i18n).
    aria::Property<std::string> zhLabel;
    aria::Property<std::string> enLabel;


    /// Switch language (the View passes a language code, e.g. "zh-CN" / "en").
    aria::Command<std::string> switchLanguage;

    void on_activate() override;
    void on_deactivate() override;
};

}  // namespace wb::settings
