#pragma once

// Workbench policy only: source-location domains, language shortcuts and the
// common namespace. All calls name an explicitly owned locale context.
#include "infra/i18n/II18nService.h"

#include <memory>
#include <source_location>
#include <string>
#include <string_view>

namespace wb::i18n {

using Context = std::shared_ptr<aria::i18n::LocaleContext>;
enum class Lang { System, ZhCN, En };
[[nodiscard]] std::string_view lang_code(Lang lang);
constexpr std::string_view kGlobalModule = "common";

[[nodiscard]] Context make_context(std::string resource_root,
                                   std::string initial_language = "zh-CN");

namespace detail {
// This repository convention is deliberately not part of Aria's API.
constexpr std::string_view module_of(std::string_view path) {
    constexpr std::string_view marker = "modules";
    std::size_t pos = 0;
    while ((pos = path.find(marker, pos)) != std::string_view::npos) {
        const auto start = pos + marker.size();
        const bool left = pos == 0 || path[pos - 1] == '/' || path[pos - 1] == '\\';
        if (left && start < path.size() && (path[start] == '/' || path[start] == '\\')) {
            const auto name_start = start + 1;
            auto end = name_start;
            while (end < path.size() && path[end] != '/' && path[end] != '\\') ++end;
            return path.substr(name_start, end - name_start);
        }
        pos = start;
    }
    return {};
}
}  // namespace detail

[[nodiscard]] inline std::string str(
    aria::i18n::LocaleContext& context, std::string_view key,
    Lang lang = Lang::System,
    std::source_location loc = std::source_location::current()) {
    const auto domain = detail::module_of(loc.file_name());
    return context.tr(domain.empty() ? kGlobalModule : domain, key, lang_code(lang));
}

[[nodiscard]] inline std::string str_in(
    aria::i18n::LocaleContext& context, std::string_view domain,
    std::string_view key, Lang lang = Lang::System) {
    return context.tr(domain.empty() ? kGlobalModule : domain, key, lang_code(lang));
}

}  // namespace wb::i18n
