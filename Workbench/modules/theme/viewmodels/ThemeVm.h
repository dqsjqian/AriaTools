#pragma once
//
// ThemeVm — Tab 8: theme switcher (DI Container)
// Three ITheme implementations (Light / Dark / Solarized). Switching
// the ComboBox re-registers the binding in the Container, and the next
// resolve<ITheme>() returns the currently selected implementation.
//
// Theme ids ("light"/"dark"/"solarized") are stable across languages
// and used for VM state + persistence; display names are resolved via
// i18n at query time, so language switches refresh them automatically.
//

#include "aria/aria.hpp"
#include "aria/command.hpp"
#include "aria/runtime/container.hpp"
#include "module_api/BaseVm.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace wb::theme {

struct ITheme {
    virtual ~ITheme() = default;
    virtual std::string_view name()       const noexcept = 0;
    virtual std::string_view cardBg()     const noexcept = 0;
    virtual std::string_view cardFg()     const noexcept = 0;
    virtual std::string_view cardBorder() const noexcept = 0;
};

/// Stable id + localized display name pair, surfaced to the View layer
/// for rendering the theme picker.
struct ThemeOption {
    std::string id;
    std::string displayName;
};

class ThemeVm final : public wb::core::BaseVm {
public:
    ThemeVm();

    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    aria::Property<std::string> themeLightLabel;
    aria::Property<std::string> themeDarkLabel;
    aria::Property<std::string> themeSolarizedLabel;
    aria::Property<std::string> cardTitleLabel;
    aria::Property<std::string> cardBodyLabel;

    /// Currently selected theme id (stable across languages).
    aria::Property<std::string> currentId{"light"};
    /// Localized display name of the current theme (refreshes on
    /// language change via the View layer's relocalize hook).
    aria::Property<std::string> currentDisplayName;

    void pick(const std::string& id);
    [[nodiscard]] std::shared_ptr<ITheme> theme() const;

    /// Theme picker commands — each selects a fixed theme. Drives the
    /// same pick() path as the Qt ComboBox; lets every platform use
    /// the same command surface (UIButton / ComboBox→command).
    aria::Command<> pickLight    {[this]{ pick("light");     }};
    aria::Command<> pickDark     {[this]{ pick("dark");      }};
    aria::Command<> pickSolarized{[this]{ pick("solarized"); }};

    /// All available themes (id + localized name). Called by the View
    /// when rebuilding the picker; re-query on language change.
    [[nodiscard]] std::vector<ThemeOption> available_themes() const;

    /// Resolve the localized display name for a given id.
    [[nodiscard]] std::string display_name(const std::string& id) const;

private:
    std::shared_ptr<aria::runtime::Container> container_;
};

}  // namespace wb::theme
