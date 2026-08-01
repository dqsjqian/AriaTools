#pragma once
//
// XmlI18nService — XML-based implementation of II18nService (core layer, no Qt).
//
// Adaptive design (no changes to this class needed for new modules/languages):
//   * Module-adaptive: each subdirectory name under i18n/ is a "module"; tr()
//     lazily loads that module's xml on first access, so adding a module is just
//     creating a directory + xml — zero changes to the underlying layer.
//   * Language-adaptive: available_languages() scans each module directory's
//     strings[_xx].xml filenames to infer available languages;
//     strings.xml = default language (defaultLang), strings_<lang>.xml = <lang>.
//     Adding a language is just adding a strings_<lang>.xml — zero changes to the underlying layer.
//
#include "infra/i18n/II18nService.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wb::services {

class XmlI18nService final : public II18nService {
public:
    explicit XmlI18nService(std::string baseDir,
                            std::string defaultLang = "zh-CN",
                            std::string initialLang = "zh-CN");

    aria::Property<std::string>& language() override { return language_; }
    std::vector<std::string> available_languages() const override;
    void set_language(const std::string& lang) override;
    std::string tr(std::string_view module, std::string_view key) const override;
    std::string tr_in(std::string_view lang, std::string_view module,
                      std::string_view key) const override;
    std::optional<std::string> find_in(std::string_view lang, std::string_view module,
                                       std::string_view key) const override;

private:
    using Table        = std::unordered_map<std::string, std::string>;  // key->text
    using ModuleTables = std::unordered_map<std::string, Table>;        // module->Table

    std::string baseDir_;
    std::string defaultLang_;
    aria::Property<std::string> language_;

    // lang -> (module -> (key -> text)). Lazily populated. mutable for const tr() caching.
    mutable std::unordered_map<std::string, ModuleTables> cache_;
    mutable std::mutex mutex_;

    // Lazy load: ensure (lang, module) has been read from disk into the cache.
    void ensure_module_loaded_(const std::string& lang, const std::string& module) const;
    static Table parse_file_(const std::string& path);
    std::string file_for_(const std::string& lang, const std::string& module) const;

    const std::string* lookup_(const std::string& lang,
                               std::string_view module,
                               std::string_view key) const;
};

}  // namespace wb::services
