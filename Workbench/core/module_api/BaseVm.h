#pragma once
//
// BaseVm — 所有业务 ViewModel 的基类。集中承载「对每个业务 VM 都普适」的底层能力。
// 目前只做一件横切的事：语种切换时自动刷新界面文案。
//
// 设计原则：只有极其公共、与具体业务无关的能力才放这里（如：语种变更）。
// 主题色变更等「纯表现层」能力不放 VM——那属于 View 层（各平台原生换肤），
// 应由独立的 View 侧机制/基类承载，避免把表现细节泄漏进跨平台逻辑。
//
// i18n 用法（随心所欲地在 VM 里写，均自动随语言刷新）：
//
//   // 静态文案：一行搞定，模块自动推断，切语言自动更新。
//   text(title, "title");
//   text(addLabel, "add");
//
//   // 动态文案：注册一段重算闭包，切语言自动重跑；其它触发点也可手动再调。
//   localize([this]{
//       status.set(wb::i18n::str("count_prefix") + std::to_string(n) +
//                  wb::i18n::str("count_suffix"));
//   });
//
// 两者都会「立即执行一次」设初值，之后语言变化时由基类自动重跑。无需 track、
// 无需写 module id、无需集中到某个 relocalize 方法。
//
#include "aria/binding/view_model.hpp"
#include "aria/property.hpp"
#include "infra/i18n/I18n.h"

#include <functional>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

namespace wb::core {

class BaseVm : public aria::binding::ViewModel {
public:
    BaseVm() {
        // 语言切换 → 重跑所有本地化闭包。订阅随本 VM 生命周期。
        track(wb::i18n::on_language_changed(
            [this](const std::string&) { relocalize_all_(); }));
    }

protected:
    /// 注册一段本地化闭包：立即执行一次设初值，语言切换时自动重跑。
    /// 用于动态文案（含变量拼接）或任意需要随语言刷新的逻辑。
    void localize(std::function<void()> fn) {
        fn();
        localizers_.push_back(std::move(fn));
    }

    /// 便捷：把某文案 Property 绑定到某 key。模块由调用点源文件自动推断。
    /// 等价于 localize([&]{ prop.set(wb::i18n::str_in(module, key)); })。
    void text(aria::Property<std::string>& prop, std::string key,
              std::source_location loc = std::source_location::current()) {
        std::string module{wb::i18n::detail::module_of(loc.file_name())};
        localize([&prop, module = std::move(module), key = std::move(key)]() {
            prop.set(wb::i18n::str_in(module, key));
        });
    }

private:
    void relocalize_all_() {
        for (auto& fn : localizers_) fn();
    }

    std::vector<std::function<void()>> localizers_;
};

}  // namespace wb::core
