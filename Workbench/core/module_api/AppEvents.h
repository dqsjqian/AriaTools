#pragma once
//
// AppEvents — 跨模块通信的事件类型（经 aria EventBus 发布/订阅）。
// 模块之间不直接依赖，只依赖这些事件契约。
//
#include <string>

namespace wb::events {

/// 一篇笔记被保存（notes 模块发布 → sync 模块可据此自动同步）。
struct NoteSaved {
    std::string id;
};

/// 请求执行一次数据同步（任意模块可发布 → sync 模块执行）。
struct SyncRequested {
    std::string reason;
};

/// 语言已切换（settings 模块发布 → 需要的模块/外壳可响应）。
struct LanguageChanged {
    std::string lang;
};

}  // namespace wb::events
