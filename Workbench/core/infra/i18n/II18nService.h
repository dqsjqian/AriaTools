#pragma once
//
// II18nService — Low-level i18n service (cross-platform, core layer, no platform UI dependencies).
//
// Design:
//   * Text is organized per "module" in files: i18n/<module>/strings.xml (default zh-CN), strings_en.xml.
//   * In memory managed as module -> { key -> text }, to avoid a single file growing too large.
//   * tr(module, key): look up current language -> fall back to default language (zh-CN) on miss -> return "[module/key]" if still missing.
//   * language is a reactive Property: switching refreshes immediately; VMs observe it to recompute UI text Properties,
//     which then propagate to the View via bindings (the View never queries the table directly — text is always dispatched by the VM).
//
#include "aria/property.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wb::services {

class II18nService {
public:
    virtual ~II18nService() = default;

    /// Current language code (e.g. "zh-CN" / "en"). Reactive: changes notify listeners.
    [[nodiscard]] virtual aria::Property<std::string>& language() = 0;

    /// Available language list (e.g. {"zh-CN","en"}).
    [[nodiscard]] virtual std::vector<std::string> available_languages() const = 0;

    /// Switch language: reloads and triggers language.on_changed.
    virtual void set_language(const std::string& lang) = 0;

    /// Get the text for a key in a module. Falls back to default language on miss, then returns "[module/key]" if still missing.
    [[nodiscard]] virtual std::string tr(std::string_view module,
                                         std::string_view key) const = 0;

    /// Get text in a "specified language" (does not change the current language setting). Empty lang is equivalent to tr().
    /// Falls back to default language on miss, then returns "[module/key]" if still missing.
    [[nodiscard]] virtual std::string tr_in(std::string_view lang,
                                            std::string_view module,
                                            std::string_view key) const = 0;

    /// Exact lookup: returns text on hit, nullopt on miss (no fallback, no placeholder string).
    /// Lets the upper layer implement custom fallback strategies (e.g. module -> global). Empty lang = current language.
    [[nodiscard]] virtual std::optional<std::string> find_in(
        std::string_view lang, std::string_view module, std::string_view key) const = 0;
};

}  // namespace wb::services
