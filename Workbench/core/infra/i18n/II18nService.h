#pragma once
//
// II18nService — 多语言底层服务（跨平台，core 层，无任何平台 UI 依赖）。
//
// 设计：
//   * 文案按「模块」分文件：i18n/<module>/strings.xml（默认 zh-CN）、strings_en.xml。
//   * 内存中以 module → { key → text } 管理，避免单文件过大。
//   * tr(module, key)：查当前语言 → 缺失回退默认语言(zh-CN) → 再缺返回 "[module/key]"。
//   * language 是响应式 Property：切换即时刷新，VM 监听它重算界面文案 Property，
//     再由绑定推给 View（View 不直接查表——文案统一由 VM 派发）。
//
#include "aria/property.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wb::services {

class II18nService {
public:
    virtual ~II18nService() = default;

    /// 当前语言代码（如 "zh-CN" / "en"）。响应式：切换会通知监听者。
    [[nodiscard]] virtual aria::Property<std::string>& language() = 0;

    /// 可用语言列表（如 {"zh-CN","en"}）。
    [[nodiscard]] virtual std::vector<std::string> available_languages() const = 0;

    /// 切换语言：重新加载并触发 language.on_changed。
    virtual void set_language(const std::string& lang) = 0;

    /// 取模块内某 key 的文案。缺失回退默认语言，再缺返回 "[module/key]"。
    [[nodiscard]] virtual std::string tr(std::string_view module,
                                         std::string_view key) const = 0;

    /// 取「指定语种」的文案（不改变当前语种设置）。lang 为空串时等价于 tr()。
    /// 缺失回退默认语言，再缺返回 "[module/key]"。
    [[nodiscard]] virtual std::string tr_in(std::string_view lang,
                                            std::string_view module,
                                            std::string_view key) const = 0;

    /// 精确查找：命中返回文案，未命中返回 nullopt（不回退、不产生占位串）。
    /// 供上层做「模块→全局」等自定义回退策略。lang 空串=当前语种。
    [[nodiscard]] virtual std::optional<std::string> find_in(
        std::string_view lang, std::string_view module, std::string_view key) const = 0;
};

}  // namespace wb::services
