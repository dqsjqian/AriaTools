#include "infra/i18n/I18n.h"

namespace wb::i18n {

namespace {
wb::services::II18nService* g_backend = nullptr;  // Injected by ServiceHub; valid for process lifetime
}

std::string_view lang_code(Lang lang) {
    switch (lang) {
        case Lang::System: return {};
        case Lang::ZhCN:   return "zh-CN";
        case Lang::En:     return "en";
    }
    return {};
}

void set_backend(wb::services::II18nService* backend) { g_backend = backend; }

namespace detail {

std::string resolve(std::string_view module, std::string_view key, Lang lang) {
    const std::string_view code = lang_code(lang);
    if (g_backend) {
        // 1) Current module (inferred; may be empty)
        if (!module.empty()) {
            if (auto v = g_backend->find_in(code, module, key)) return *v;
        }
        // 2) Global common — never queries other business modules
        if (module != kGlobalModule) {
            if (auto v = g_backend->find_in(code, kGlobalModule, key)) return *v;
        }
    }
    // 3) Placeholder, helps locate missing text
    std::string tag(module.empty() ? kGlobalModule : module);
    return "[" + tag + "/" + std::string(key) + "]";
}

}  // namespace detail

std::string language() {
    return g_backend ? g_backend->language().get() : std::string{};
}

void set_language(const std::string& lang) {
    if (g_backend) g_backend->set_language(lang);
}

aria::Subscription on_language_changed(std::function<void(const std::string&)> fn) {
    if (!g_backend) return {};
    return g_backend->language().on_changed(std::move(fn));
}

}  // namespace wb::i18n
