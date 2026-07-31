#pragma once
// Qt View 注册清单：显式调用各模块的 register_<mod>_view()。
// 加模块 = 这里加一行（与 core 的 ModulesManifest 对称）。
namespace wb::qt { void register_all_views(); }
