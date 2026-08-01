#pragma once
//
// I18n — Global i18n facade (infra layer). Zero ceremony:
//   * No need to inherit an i18n base class, inject II18nService, or write module id at each call site;
//   * Module is auto-inferred from the call site's source file path (.../modules/<module>/...);
//   * Lookup order: current module -> global common; if not found, returns a placeholder. **Never crosses to other modules.**
//
// Business usage (final form):
//   auto s = wb::i18n::str("title");            // current module, returns std::string
//   title.set(wb::i18n::str("title"));          // feed directly to a Property
//   auto en = wb::i18n::str("save", Lang::En);   // specify a language
//
// Auto-refresh on language change: have the VM inherit wb::core::BaseVm and use
//   text(prop,"key") or localize([&]{ ... }) to write text near its use site;
//   it re-runs automatically on language change.
//
// The backend is injected once by ServiceHub at startup (symmetric with wb::log).
//
#include "infra/i18n/II18nService.h"

#include "aria/subscription.hpp"

#include <functional>
#include <source_location>
#include <string>
#include <string_view>

namespace wb::i18n {

/// Language enum. System = follow global setting; others are concrete languages.
enum class Lang { System, ZhCN, En };

[[nodiscard]] std::string_view lang_code(Lang lang);

/// Inject the backend from ServiceHub (the II18nService in the DI container).
void set_backend(wb::services::II18nService* backend);

/// The global "module" for common text (cross-module shared, e.g. generic button labels).
constexpr std::string_view kGlobalModule = "common";

namespace detail {

/// Infer module name from a source file path: extracts <module> from ".../modules/<module>/...".
/// Accepts both '/' and '\\'. Returns empty string if inference fails (then only global common is queried).
constexpr std::string_view module_of(std::string_view path) {
    constexpr std::string_view marker = "modules";
    const auto pos = path.find(marker);
    if (pos == std::string_view::npos) return {};
    auto start = pos + marker.size();
    if (start >= path.size() || (path[start] != '/' && path[start] != '\\')) return {};
    ++start;
    auto end = start;
    while (end < path.size() && path[end] != '/' && path[end] != '\\') ++end;
    if (end == start) return {};
    return path.substr(start, end - start);
}

/// Core lookup: module -> global(common) -> placeholder. Never queries other modules.
[[nodiscard]] std::string resolve(std::string_view module, std::string_view key,
                                  Lang lang);

}  // namespace detail

// ── Main business entry: fetch text by key, module auto-inferred (call site source_location) ──
// Default argument is evaluated at the call site, so it captures the caller's file path to infer the module.
[[nodiscard]] inline std::string str(
    std::string_view key,
    Lang lang = Lang::System,
    std::source_location loc = std::source_location::current()) {
    return detail::resolve(detail::module_of(loc.file_name()), key, lang);
}

/// Explicitly specify the module (rare cross-module/tooling scenarios; usually unnecessary).
[[nodiscard]] inline std::string str_in(
    std::string_view module, std::string_view key, Lang lang = Lang::System) {
    return detail::resolve(module, key, lang);
}

// ── Current language / switch / subscribe ──────────────────────────────────
[[nodiscard]] std::string language();
void set_language(const std::string& lang);
[[nodiscard]] aria::Subscription on_language_changed(
    std::function<void(const std::string&)> fn);

}  // namespace wb::i18n
