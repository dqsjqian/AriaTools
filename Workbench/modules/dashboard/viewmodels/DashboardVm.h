#pragma once
//
// DashboardVm — home overview. Text is dispatched by the VM via i18n (View has zero string literals).
//
// Cross-module: subscribes to ItemAddedToCart / OrderPlaced events from the
// EventBus so the home page can show a live "cart: N items" badge without
// any direct coupling to the cart module.
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "aria/subscription.hpp"

namespace wb::dashboard {

class DashboardVm final : public wb::core::BaseVm {
public:
    DashboardVm();

    aria::Property<std::string> welcome;   ///< Welcome text (updates on language change)
    aria::Property<std::string> summary;   ///< Summary text (updates on language change)

    // ── Cross-module live state ────────────────────────────────────────
    /// "Cart: N items" badge text, refreshed by ItemAddedToCart events.
    aria::Property<std::string> cartBadge;
    /// Last order confirmation, refreshed by OrderPlaced events.
    aria::Property<std::string> lastOrder;
    /// Total cart items (int, for badge rendering).
    aria::Property<int> cartItemCount{0};

    void on_activate() override;
    void on_deactivate() override;

private:
    // Cross-module subscriptions live for the VM's entire lifetime (not in
    // bag()) so the badge stays current even when the user is on another tab.
    aria::Subscription lang_sub_;
    aria::Subscription cart_sub_;
    aria::Subscription order_sub_;
    aria::Subscription qty_sub_;
};

}  // namespace wb::dashboard
