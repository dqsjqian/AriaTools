#pragma once
//
// CrossModuleEvents.h — shared event types for cross-module communication
// via aria::runtime::EventBus.
//
// These events are defined in the _shared/ namespace so any module can
// publish or subscribe to them. The EventBus is process-wide (accessed
// via ModuleContext::bus()), so a publish in module A can be received
// by a subscription in module B — no direct coupling between the two.
//
// Demo scenario:
//   cart module      → publishes ItemAddedToCart when user adds an item
//   dashboard module → subscribes, shows a "cart: N items" badge
//   chat module      → subscribes, auto-posts a system message
//
#include <string>

namespace wb::shared::events {

/// Fired when the user adds an item to the shopping cart.
struct ItemAddedToCart {
    std::string productName;
    double     price = 0.0;
    int        qty = 1;
};

/// Fired when the user places an order (checkout).
struct OrderPlaced {
    std::string orderId;
    double     total = 0.0;
    int        itemCount = 0;
};

/// Fired when the user switches the UI language.
struct LanguageChanged {
    std::string languageCode;  // e.g. "zh-CN", "en"
};

}  // namespace wb::shared::events
