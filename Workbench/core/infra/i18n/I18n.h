#pragma once
//
// I18n — 全局多语言门面（infra 层）。零仪式：
//   * 不需继承 i18n 基类、不需注入 II18nService、不需在每处写 module id；
//   * module 由调用点源文件路径自动推断（.../modules/<module>/...）；
//   * 查找顺序：当前模块 → 全局 common，取不到再返回占位；**绝不跨到其他模块**。
//
// 业务写法（最终形态）：
//   auto s = wb::i18n::str("title");            // 当前模块，返回 std::string
//   title.set(wb::i18n::str("title"));          // 直接喂给 Property
//   auto en = wb::i18n::str("save", Lang::En);   // 指定语种
//
// 自动随语言刷新：让 VM 继承 wb::core::BaseVm，用 text(prop,"key") 或
//   localize([&]{ ... }) 就近书写文案，语言切换时自动重跑。
//
// 后端由 ServiceHub 启动时注入一次（与 wb::log 同构）。
//
#include "infra/i18n/II18nService.h"

#include "aria/subscription.hpp"

#include <functional>
#include <source_location>
#include <string>
#include <string_view>

namespace wb::i18n {

/// 语种枚举。System=跟随全局设置；其余为具体语种。
enum class Lang { System, ZhCN, En };

[[nodiscard]] std::string_view lang_code(Lang lang);

/// 由 ServiceHub 注入后端（DI 容器里的 II18nService）。
void set_backend(wb::services::II18nService* backend);

/// 全局公共文案所在「模块」（跨模块共享，如通用按钮文案）。
constexpr std::string_view kGlobalModule = "common";

namespace detail {

/// 从源文件路径推断模块名：取 ".../modules/<module>/..." 中的 <module>。
/// 兼容 '/' 与 '\\'。推断不出时返回空串（则只查全局 common）。
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

/// 核心查询：module → global(common) → 占位。绝不查其他模块。
[[nodiscard]] std::string resolve(std::string_view module, std::string_view key,
                                  Lang lang);

}  // namespace detail

// ── 业务主入口：按 key 取文案，模块自动推断（调用点 source_location）──────────
// 默认实参在「调用点」求值，故能拿到调用处的文件路径来推断模块。
[[nodiscard]] inline std::string str(
    std::string_view key,
    Lang lang = Lang::System,
    std::source_location loc = std::source_location::current()) {
    return detail::resolve(detail::module_of(loc.file_name()), key, lang);
}

/// 显式指定模块（少数跨模块/工具场景；日常无需）。
[[nodiscard]] inline std::string str_in(
    std::string_view module, std::string_view key, Lang lang = Lang::System) {
    return detail::resolve(module, key, lang);
}

// ── 当前语种 / 切换 / 订阅 ───────────────────────────────────────────────────
[[nodiscard]] std::string language();
void set_language(const std::string& lang);
[[nodiscard]] aria::Subscription on_language_changed(
    std::function<void(const std::string&)> fn);

}  // namespace wb::i18n
