#pragma once
//
// ModulesManifest — 显式列举要装配进 app 的模块工厂。
//
// 为什么显式（而非全局自注册）：模块是独立静态库，全局构造函数自注册会被链接器
// 裁剪，需要 -force_load/whole-archive 且跨平台易坏。显式 manifest 更可靠，
// 且"加一个模块 = 这里加一行"，模块本身仍自闭环、可独立构建。
//
#include "module_api/ModuleRegistry.h"

namespace wb::app {

/// 把所有启用的模块注册进 registry（按各自 order 排序后使用）。
void populate_modules(wb::module_api::ModuleRegistry& registry);

}  // namespace wb::app
