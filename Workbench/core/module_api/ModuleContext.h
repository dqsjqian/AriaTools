#pragma once
//
// ModuleContext — 注入给每个模块的上下文。
// 模块经此拿到稳定服务（ServiceHub）与事件总线，不自行 new、不认识具体实现。
//
#include "infra/ServiceHub.h"
#include "aria/runtime/event_bus.hpp"

namespace wb::module_api {

class ModuleContext {
public:
    explicit ModuleContext(wb::infra::ServiceHub& hub) : hub_(hub) {}

    [[nodiscard]] wb::infra::ServiceHub& services() { return hub_; }
    [[nodiscard]] aria::runtime::EventBus& bus() { return hub_.bus(); }

    /// 直取某稳定服务接口。
    template <typename I>
    [[nodiscard]] I& service() { return hub_.service<I>(); }

private:
    wb::infra::ServiceHub& hub_;
};

}  // namespace wb::module_api
