#include "viewmodels/DashboardVm.h"

#include "infra/i18n/I18n.h"
#include "events/CrossModuleEvents.h"

#include "module_api/NavigationEntryVm.h"

#include "aria/runtime/event_bus.hpp"

namespace wb::dashboard {

DashboardVm::DashboardVm(wb::module_api::ModuleContext& ctx)
    : navigator_(&ctx.navigator()),
      mounts_(&ctx.mounts()),
      modalCart([this] {
          // Same navigation, presented as a modal dialog: the View renders
          // the entry as an overlay (Qt QDialog / iOS present / Compose Dialog).
          navigator_->Push<wb::module_api::ICartPage>(
              wb::module_api::CartArgs{.product = "Apple", .price = 2.5},
              wb::module_api::NavOptions{
                  .presentation = wb::module_api::Presentation::Modal});
      }),
      windowCart([this] {
          // Same navigation, presented as a standalone top-level window
          // (desktop shells). Mobile shells fall back to a modal.
          navigator_->Push<wb::module_api::ICartPage>(
              wb::module_api::CartArgs{.product = "Apple", .price = 2.5},
              wb::module_api::NavOptions{
                  .presentation = wb::module_api::Presentation::Window});
      }),
      navBack([this] { navigator_->Pop(); }),
      mountToggle([this] {
          // Flip the extension on/off. The cart module registered the slot's
          // provider factory at load time; we only toggle the switch — no
          // knowledge of the provider needed (decoupled both ways).
          const bool enable = mountedModule.get().empty();
          mounts_->SetEnabled(wb::module_api::slots::kDashboardContent, enable);
          sync_mount_state();
      })
{
    text(welcome, "welcome");
    text(summary, "summary");
    text(modalCartLabel, "modal_cart");
    text(windowCartLabel, "window_cart");
    text(navBackLabel, "back");
    text(mountToggleLabel, "mount_toggle");
    // Seed the badge with zero items.
    cartBadge.set(wb::i18n::str_in("dashboard", "cart_empty"));
    lastOrder.set(wb::i18n::str_in("dashboard", "no_orders"));

    // Seed the extension state from what the cart module registered at
    // load time (dashboard.content is provided by cart's register_mounts).
    sync_mount_state();

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
        navPresentation.set(entry
                                ? static_cast<int>(entry->presentation())
                                : 0);
        navDepth.set(static_cast<int>(depth.get()));
    };
    nav_sync_sub_ = depth.on_changed([sync_nav](const auto&) { sync_nav(); });
    sync_nav();
}

void DashboardVm::sync_mount_state() {
    const auto mod =
        mounts_->module_of(wb::module_api::slots::kDashboardContent);
    mountedModule.set(mod.value_or(""));
    if (mod) {
        // Extension points carry parameters via TWO channels (mirroring
        // navigation): typed struct — Resolve<IMountCartArgs>(slot,
        // CartArgs{...}) — and free-form json. The dashboard uses the typed
        // channel to pre-fill the mounted cart with a product; a host would
        // normally pass its own state.
        auto m = mounts_->Resolve<wb::module_api::IMountCartArgs>(
            wb::module_api::slots::kDashboardContent,
            wb::module_api::CartArgs{.product = "Mounted Apple",
                                     .price   = 7.5});
        mountedVm.set(m ? std::move(m->vm) : nullptr);
        mountStatus.set(wb::i18n::str_in("dashboard", "mount_mounted")
                        + " " + *mod);
    } else {
        mountedVm.set(nullptr);
        mountStatus.set(wb::i18n::str_in("dashboard", "mount_empty"));
    }
}

}  // namespace wb::dashboard
