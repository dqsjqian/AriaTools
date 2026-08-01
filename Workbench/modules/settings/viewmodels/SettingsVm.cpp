#include "viewmodels/SettingsVm.h"

#include "infra/i18n/I18n.h"

namespace wb::settings {

SettingsVm::SettingsVm()
    : language(wb::i18n::language()),
      switchLanguage([this](const std::string& lang) {
          wb::i18n::set_language(lang);
          language.set(lang);
      })
{
    text(title,         "title");
    text(hint,          "hint");
    text(languageLabel, "language");
}

void SettingsVm::on_activate() { language.set(wb::i18n::language()); }
void SettingsVm::on_deactivate() { bag().clear(); }

}  // namespace wb::settings
