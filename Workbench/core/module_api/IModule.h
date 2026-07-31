#pragma once
//
// IModule — 业务模块插件契约（编译期）。
// 每个模块实现之，并导出 make_<mod>_module() 工厂。app 层 ModulesManifest 显式列举。
//
#include "aria/binding/view_model.hpp"
#include "module_api/ModuleContext.h"

#include <memory>
#include <string>

namespace wb::module_api {

class IModule {
public:
    virtual ~IModule() = default;

    /// 模块 id（= i18n 子目录名，如 "notes"）。
    [[nodiscard]] virtual std::string id() const = 0;

    /// 导航文案 key（在 common 模块内，如 "nav_notes"）。
    [[nodiscard]] virtual std::string nav_key() const = 0;

    /// 导航顺序（升序）。
    [[nodiscard]] virtual int order() const = 0;

    /// 创建本模块的 ViewModel（注入服务/事件总线）。
    [[nodiscard]] virtual std::shared_ptr<aria::binding::ViewModel>
        create_view_model(ModuleContext& ctx) = 0;
};

}  // namespace wb::module_api
