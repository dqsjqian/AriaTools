#include "viewmodels/ThemeVm.h"

#include "infra/i18n/I18n.h"

namespace wb::theme {

namespace {

struct LightTheme : ITheme {
    std::string_view name()       const noexcept override { return "Light"; }
    std::string_view cardBg()     const noexcept override { return "#ffffff"; }
    std::string_view cardFg()     const noexcept override { return "#212121"; }
    std::string_view cardBorder() const noexcept override { return "#e0e0e0"; }
};

struct DarkTheme : ITheme {
    std::string_view name()       const noexcept override { return "Dark"; }
    std::string_view cardBg()     const noexcept override { return "#263238"; }
    std::string_view cardFg()     const noexcept override { return "#eceff1"; }
    std::string_view cardBorder() const noexcept override { return "#455a64"; }
};

struct SolarizedTheme : ITheme {
    std::string_view name()       const noexcept override { return "Solarized"; }
    std::string_view cardBg()     const noexcept override { return "#fdf6e3"; }
    std::string_view cardFg()     const noexcept override { return "#586e75"; }
    std::string_view cardBorder() const noexcept override { return "#eee8d5"; }
};

/// Internal id ↔ implementation mapping. The id is stable across
/// languages (used by VM state and persistence); the *display* name
/// is resolved via i18n at query time by ThemeVm::display_name().
struct ThemeEntry {
    std::string id;
    std::shared_ptr<ITheme> impl;
};

std::vector<ThemeEntry> all_themes() {
    return {
        {"light",     std::make_shared<LightTheme>()},
        {"dark",      std::make_shared<DarkTheme>()},
        {"solarized", std::make_shared<SolarizedTheme>()},
    };
}

const char* i18n_key_for(const std::string& id) {
    if (id == "light")     return "theme_light";
    if (id == "dark")      return "theme_dark";
    if (id == "solarized") return "theme_solarized";
    return "theme_light";
}

}  // namespace

ThemeVm::ThemeVm() : container_(std::make_shared<aria::runtime::Container>()) {
    container_->register_singleton<ITheme, LightTheme>();
}

void ThemeVm::pick(const std::string& id) {
    container_->clear();
    if      (id == "light")     container_->register_singleton<ITheme, LightTheme>();
    else if (id == "dark")      container_->register_singleton<ITheme, DarkTheme>();
    else if (id == "solarized") container_->register_singleton<ITheme, SolarizedTheme>();
    currentId.set(id);
    // Refresh the localized display name whenever the selection changes.
    currentDisplayName.set(display_name(id));
}

std::shared_ptr<ITheme> ThemeVm::theme() const {
    return container_->resolve<ITheme>();
}

std::vector<ThemeOption> ThemeVm::available_themes() const {
    std::vector<ThemeOption> out;
    out.reserve(3);
    for (const auto& e : all_themes()) {
        out.push_back({e.id, wb::i18n::str_in("theme", i18n_key_for(e.id))});
    }
    return out;
}

std::string ThemeVm::display_name(const std::string& id) const {
    return wb::i18n::str_in("theme", i18n_key_for(id));
}

}  // namespace wb::theme
