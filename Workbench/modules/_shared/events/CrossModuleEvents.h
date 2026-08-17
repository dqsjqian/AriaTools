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
// Demo scenario (3 modules reacting to cart operations):
//   cart module      → publishes ItemAddedToCart / ItemQtyChanged / OrderPlaced
//   dashboard module → subscribes, shows a live "cart: N items" badge
//   chat module      → subscribes, auto-posts system messages
//   notes module     → subscribes, auto-creates operation-log notes
//
// The ItemQtyChanged event demonstrates Model → EventBus propagation:
// when a CartItem's qty Property changes, CartVm forwards it as an event,
// and multiple modules' VMs receive the notification simultaneously.
//
#include <string>

namespace wb::shared::events {

/// Fired when the user adds an item to the shopping cart.
struct ItemAddedToCart {
    std::string productName;
    double     price = 0.0;
    int        qty = 1;
};

/// Fired when an existing cart item's quantity changes (model → event).
/// Demonstrates that a Model-level change (CartItem::qty) propagates to
/// multiple modules' VMs via the EventBus.
struct ItemQtyChanged {
    std::string productName;
    int        oldQty = 0;
    int        newQty = 0;
};

/// Fired on EVERY cart mutation (add / remove / qty change / checkout).
/// Carries the CURRENT item count so subscribers can adopt it directly
/// instead of maintaining their own running sum (which goes stale when
/// items are removed — the counting bug we fixed). Subscribers that need
/// richer detail also listen to ItemAddedToCart / ItemQtyChanged.
struct CartStateChanged {
    int itemCount = 0;
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
