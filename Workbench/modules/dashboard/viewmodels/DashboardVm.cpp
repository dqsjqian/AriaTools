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
    cart_sub_ = bus.subscribe<wb::shared::events::ItemAddedToCart>(
        [this](const wb::shared::events::ItemAddedToCart& ev) {
            int n = cartItemCount.get() + ev.qty;
            cartItemCount.set(n);
            cartBadge.set(wb::i18n::str_in("dashboard", "cart_items")
                         + " " + std::to_string(n));
        });
    order_sub_ = bus.subscribe<wb::shared::events::OrderPlaced>(
        [this](const wb::shared::events::OrderPlaced& ev) {
            lastOrder.set(wb::i18n::str_in("dashboard", "order_placed")
                         + ": " + ev.orderId
                         + " (" + std::to_string(ev.itemCount) + " items, "
                         + std::to_string(ev.total) + ")");
            cartItemCount.set(0);
            cartBadge.set(wb::i18n::str_in("dashboard", "cart_empty"));
        });
    qty_sub_ = bus.subscribe<wb::shared::events::ItemQtyChanged>(
        [this](const wb::shared::events::ItemQtyChanged& ev) {
            // Model → EventBus → dashboard VM: a cart item's qty changed.
            // Update the badge to reflect the latest count.
            int n = cartItemCount.get();
            // qty change doesn't change total count (it's a swap, not add)
            // but we log it for the demo.
            lastOrder.set(wb::i18n::str_in("dashboard", "qty_changed")
                         + ": " + ev.productName);
            (void)n;
        });
}

void DashboardVm::on_activate() {}
void DashboardVm::on_deactivate() { bag().clear(); }

}  // namespace wb::dashboard
