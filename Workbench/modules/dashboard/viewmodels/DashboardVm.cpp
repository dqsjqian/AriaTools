#include "viewmodels/DashboardVm.h"

#include "infra/i18n/I18n.h"
#include "events/CrossModuleEvents.h"

#include "aria/runtime/event_bus.hpp"

namespace wb::dashboard {

DashboardVm::DashboardVm()
{
    text(welcome, "welcome");
    text(summary, "summary");
    // Seed the badge with zero items.
    cartBadge.set(wb::i18n::str_in("dashboard", "cart_empty"));
    lastOrder.set(wb::i18n::str_in("dashboard", "no_orders"));

    // Cross-module: subscribe to cart events so the home badge updates live
    // whenever the user adds items in the cart module (no direct coupling).
    // Subscription is kept on a standalone member (not bag()) so it survives
    // tab switches — we want the badge to stay current even when the user
    // is on another tab.
    auto& bus = aria::runtime::EventBus::global();
    lang_sub_ = wb::i18n::on_language_changed([this](const std::string&) {
        if (cartItemCount.get() == 0) {
            cartBadge.set(wb::i18n::str_in("dashboard", "cart_empty"));
        } else {
            cartBadge.set(wb::i18n::str_in("dashboard", "cart_items")
                         + " " + std::to_string(cartItemCount.get()));
        }
    });
    cart_sub_ = bus.subscribe<wb::shared::events::CartStateChanged>(
        [this](const wb::shared::events::CartStateChanged& ev) {
            // Adopt the CURRENT count from the cart module — NOT a running
            // sum. A running sum goes stale when items are removed (the
            // counting bug: deleting a cart item didn't decrement the badge).
            cartItemCount.set(ev.itemCount);
            if (ev.itemCount == 0) {
                cartBadge.set(wb::i18n::str_in("dashboard", "cart_empty"));
            } else {
                cartBadge.set(wb::i18n::str_in("dashboard", "cart_items")
                             + " " + std::to_string(ev.itemCount));
            }
        });
    // Keep ItemAddedToCart / ItemQtyChanged subscriptions for the richer
    // "what happened" detail (chat / notes use them); the badge count itself
    // is driven by CartStateChanged above.
    order_sub_ = bus.subscribe<wb::shared::events::OrderPlaced>(
        [this](const wb::shared::events::OrderPlaced& ev) {
            lastOrder.set(wb::i18n::str_in("dashboard", "order_placed")
                         + ": " + ev.orderId
                         + " (" + std::to_string(ev.itemCount) + " items, "
                         + std::to_string(ev.total) + ")");
        });
    qty_sub_ = bus.subscribe<wb::shared::events::ItemQtyChanged>(
        [this](const wb::shared::events::ItemQtyChanged& ev) {
            lastOrder.set(wb::i18n::str_in("dashboard", "qty_changed")
                         + ": " + ev.productName);
        });
}

void DashboardVm::on_activate() {}
void DashboardVm::on_deactivate() { bag().clear(); }

}  // namespace wb::dashboard
