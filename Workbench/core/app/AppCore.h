#pragma once
//
// AppCore — 平台无关应用核心。装配 ServiceHub(稳定服务) + 遍历模块注册表建 VM。
// 不含任何 UI 适配器/BindingEngine（各端外壳持有）。
//
#include "infra/ServiceHub.h"
#include "module_api/ModuleRegistry.h"
#include "module_api/ModuleContext.h"

#include <memory>
#include <string>
#include <vector>

namespace wb::core {

/// 一个已装配的模块条目（id + 导航标题 key + VM 实例）。
struct ModuleEntry {
    std::string id;
    std::string navKey;
    std::shared_ptr<aria::binding::ViewModel> vm;
};

class AppCore {
public:
    /// i18nBaseDir：运行目录 i18n/ 路径（各模块文案汇聚于此的 <mod>/ 子目录）。
    explicit AppCore(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~AppCore();

    AppCore(const AppCore&) = delete;
    AppCore& operator=(const AppCore&) = delete;

    [[nodiscard]] const std::vector<ModuleEntry>& modules() const { return entries_; }
    [[nodiscard]] wb::infra::ServiceHub& services() { return hub_; }
    [[nodiscard]] wb::services::II18nService& i18n() { return hub_.i18n(); }

    /// 取某模块导航标题（当前语言，来自 common 模块的 nav_ key）。
    [[nodiscard]] std::string nav_title(const std::string& navKey);

private:
    wb::infra::ServiceHub                     hub_;
    wb::module_api::ModuleRegistry            registry_;
    wb::module_api::ModuleContext             ctx_;
    std::vector<ModuleEntry>                  entries_;
};

}  // namespace wb::core
