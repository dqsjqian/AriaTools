#pragma once
//
// ModuleRegistry — 已装配模块的运行时注册表（app 层用显式 manifest 填充）。
// 不依赖全局构造函数自动注册（静态库会被链接器裁剪），由 ModulesManifest 显式 add()。
//
#include "module_api/IModule.h"

#include <memory>
#include <vector>

namespace wb::module_api {

class ModuleRegistry {
public:
    /// 加入一个模块实例（由 manifest 调用）。
    void add(std::shared_ptr<IModule> m) { modules_.push_back(std::move(m)); }

    /// 按 order 升序返回所有模块。
    [[nodiscard]] std::vector<std::shared_ptr<IModule>> ordered() const;

    [[nodiscard]] const std::vector<std::shared_ptr<IModule>>& all() const {
        return modules_;
    }

private:
    std::vector<std::shared_ptr<IModule>> modules_;
};

}  // namespace wb::module_api
