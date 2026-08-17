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
    text(zhLabel,       "lang_zh_CN");
    text(enLabel,       "lang_en");
}

void SettingsVm::on_activate() { language.set(wb::i18n::language()); }


std::vector<SettingsVm::LangEntry> SettingsVm::available_languages() const {
    return {
        {"zh-CN", wb::i18n::str_in("common", "lang_zh_CN")},
        {"en",    wb::i18n::str_in("common", "lang_en")},
    };
}

}  // namespace wb::settings
