#pragma once
//
// DashboardVm — home overview. Text is dispatched by the VM via i18n (View has zero string literals).
//
// Cross-module: subscribes to ItemAddedToCart / OrderPlaced events from the
// EventBus so the home page can show a live "cart: N items" badge without
// any direct coupling to the cart module.
//
// Cross-module navigation: demonstrates typed, param-carrying routing that
// lives entirely in the VM layer. The VM only knows the *interface*
// (wb::module_api::ICartPage) from the framework kernel — it never includes
// the cart module's headers. Routing:
//     ctx.navigator().Push<ICartPage>(params)   // cart implements it
// If no module implements ICartPage (module removed), Push returns false.
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "module_api/ModuleContext.h"
#include "module_api/NavigatorHost.h"
#include "module_api/capabilities/cart/ICartPage.h"
#include "aria/subscription.hpp"

#include <memory>

namespace wb::dashboard {

class DashboardVm final : public wb::core::BaseVm {
public:
    explicit DashboardVm(wb::module_api::ModuleContext& ctx);

    aria::Property<std::string> welcome;   ///< Welcome text (updates on language change)
    aria::Property<std::string> summary;   ///< Summary text (updates on language change)

    // ── Cross-module live state ────────────────────────────────────────
    /// "Cart: N items" badge text, refreshed by ItemAddedToCart events.
    aria::Property<std::string> cartBadge;
    /// Last order confirmation, refreshed by OrderPlaced events.
    aria::Property<std::string> lastOrder;
    /// Total cart items (int, for badge rendering).
    aria::Property<int> cartItemCount{0};

    // ── Cross-module navigation (VM-layer routing) ─────────────────────
    /// Open the cart page via the typed ICartPage contract, carrying a
    /// prefill param ("product" / "price"). The View only fires the
    /// Command and renders navigator().current().
    aria::Command<> openCart;
    /// Pop the navigation stack.
    aria::Command<> navBack;

    /// The shared navigator (AppCore-owned). View binds to current()/depth().
    [[nodiscard]] wb::module_api::NavigatorHost& navigator() const {
        return *navigator_;
    }

    /// Current pushed page's module id ("" when the stack is at root).
    /// Mirrors the navigator for side-channel platforms (Android) that
    /// cannot bind the Navigator directly.
    aria::Property<std::string> navCurrentModule;
    /// Navigation stack depth (mirrors navigator().depth()).
    aria::Property<int> navDepth{0};

    /// Button labels for the navigation demo (i18n).
    aria::Property<std::string> openCartLabel;
    aria::Property<std::string> navBackLabel;

private:
    wb::module_api::NavigatorHost* navigator_ = nullptr;

    // Cross-module subscriptions live for the VM's entire lifetime (not in
    // bag()) so the badge stays current even when the user is on another tab.
    aria::Subscription lang_sub_;
    aria::Subscription cart_sub_;
    aria::Subscription order_sub_;
    aria::Subscription qty_sub_;
    aria::Subscription nav_sync_sub_;
};

}  // namespace wb::dashboard
