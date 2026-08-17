#include "viewmodels/DashboardVm.h"

#include "infra/i18n/I18n.h"
#include "events/CrossModuleEvents.h"

#include "module_api/NavigationEntryVm.h"

#include "aria/runtime/event_bus.hpp"

namespace wb::dashboard {

DashboardVm::DashboardVm(wb::module_api::ModuleContext& ctx)
    : navigator_(&ctx.navigator()),
      openCart([this] {
          // VM-layer routing with a TYPED payload: compile-time field
          // checking, serialized to json by the navigator. The View only
          // fires the Command — routing lives here in the VM.
          navigator_->Push<wb::module_api::ICartPage>(
              wb::module_api::CartArgs{.product = "Apple", .price = 2.5});
      }),
      navBack([this] { navigator_->Pop(); })
{
    text(welcome, "welcome");
    text(summary, "summary");
    text(openCartLabel, "open_cart");
    text(navBackLabel, "back");
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

    // Mirror the navigation stack to side-channel platforms (Android).
    // Subscribe to `depth` (updated *after* `current` in Navigator::publish_)
    // so both mirrors read fresh values in the same callback.
    auto& nav = navigator_->current();
    auto& depth = navigator_->depth();
    auto sync_nav = [this, &nav, &depth] {
        auto* cur = nav.get().get();
        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(cur);
        navCurrentModule.set(entry ? entry->module_id() : std::string{});
        navDepth.set(static_cast<int>(depth.get()));
    };
    nav_sync_sub_ = depth.on_changed([sync_nav](const auto&) { sync_nav(); });
    sync_nav();
}

}  // namespace wb::dashboard
