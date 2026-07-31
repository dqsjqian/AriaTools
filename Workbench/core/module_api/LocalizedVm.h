#pragma once
//
// LocalizedVm — 带多语言能力的 ViewModel 基类。
//
// 目标（呼应「逻辑全在跨平台层、View 零文案字面量」）：
//   * VM 持有 II18nService，界面文案一律作为 Property<std::string> 由 VM 派发；
//   * 注册的文案在「语言切换」时自动重算并通知 View，无需各 VM 手写监听。
//
// 用法：
//   class FooVm : public LocalizedVm {
//     FooVm(II18nService& i18n) : LocalizedVm(i18n, "foo") {
//        bind_text(title, "title");        // title 会随语言自动更新
//     }
//     aria::Property<std::string> title;
//   };
//
#include "aria/property.hpp"
#include "aria/binding/view_model.hpp"
#include "infra/i18n/II18nService.h"

#include <string>
#include <utility>
#include <vector>

namespace wb::core {

class LocalizedVm : public aria::binding::ViewModel {
public:
    LocalizedVm(wb::services::II18nService& i18n, std::string module)
        : i18n_(i18n), module_(std::move(module)) {
        // 语言变化 → 重算所有已注册文案 Property。订阅生命周期随 VM。
        lang_sub_ = i18n_.language().on_changed(
            [this](const std::string&) { relocalize_(); });
    }

    /// 取本模块某 key 的即时文案（用于非 Property 场景，如列表项拼装）。
    [[nodiscard]] std::string tr(std::string_view key) const {
        return i18n_.tr(module_, key);
    }
    /// 取任意模块（如 common）的文案。
    [[nodiscard]] std::string tr(std::string_view module, std::string_view key) const {
        return i18n_.tr(module, key);
    }

protected:
    /// 注册一个「随语言自动更新」的文案 Property：初值即 tr(key)，
    /// 语言变化时自动 set 为新语言的 tr(key)。
    void bind_text(aria::Property<std::string>& prop, std::string key) {
        prop.set(i18n_.tr(module_, key));
        entries_.push_back({&prop, std::move(key)});
    }

    [[nodiscard]] wb::services::II18nService& i18n() { return i18n_; }
    [[nodiscard]] const std::string& module() const { return module_; }

private:
    void relocalize_() {
        for (auto& e : entries_) e.prop->set(i18n_.tr(module_, e.key));
    }

    struct Entry { aria::Property<std::string>* prop; std::string key; };

    wb::services::II18nService& i18n_;
    std::string module_;
    std::vector<Entry> entries_;
    aria::Subscription lang_sub_;
};

}  // namespace wb::core
