#include "infra/i18n/I18n.h"
#include "infra/ServiceFactories.h"

namespace wb::i18n {

std::string_view lang_code(Lang lang) {
    switch (lang) {
        case Lang::System: return {};
        case Lang::ZhCN: return "zh-CN";
        case Lang::En: return "en";
    }
    return {};
}

Context make_context(std::string resource_root, std::string initial_language) {
    return Context{wb::services::make_xml_i18n_service(
        std::move(resource_root), std::move(initial_language))};
}

}  // namespace wb::i18n

namespace wb::services {

II18nService* make_xml_i18n_service(std::string baseDir, std::string initialLang) {
    auto catalog = std::make_shared<aria::i18n::XmlCatalog>(std::move(baseDir), "zh-CN");
    return new II18nService(std::move(catalog), aria::i18n::LocaleOptions{
        .language = std::move(initialLang),
        .default_language = "zh-CN",
        .common_domain = "common",
        .fallback_order = aria::i18n::FallbackOrder::DomainFirst});
}

}  // namespace wb::services
