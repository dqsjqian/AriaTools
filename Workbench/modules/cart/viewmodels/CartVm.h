#pragma once
//
// CartVm — Tab 3: shopping cart
//
// Scenario
//   Add / remove / change quantity → live aggregation of subtotal /
//   tax / total plus a "N items" badge.
//
// Cross-module: publishes ItemAddedToCart on the EventBus so other modules
// (dashboard badge, chat system message) can react without direct coupling.
//

#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "module_api/INavigationTarget.h"
#include "module_api/capabilities/cart/ICartPage.h"
#include "aria/command.hpp"
#include "aria/observable_list.hpp"
#include "aria/binding/view_model.hpp"
#include "aria/runtime/event_bus.hpp"

#include "models/CartItem.h"

#include <nlohmann/json.hpp>

#include <string>

namespace wb::cart {

class CartVm : public wb::core::BaseVm,
               public wb::module_api::ICartPage {
private:
    aria::runtime::EventBus& bus_;

public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    static constexpr double kTaxRate = 0.08;

    /// Construct with the shared EventBus (for cross-module publishing).
    explicit CartVm(aria::runtime::EventBus& bus);

    aria::ObservableList<CartItem> items;

private:
    // Declared after items so the subscription disconnects before the list is
    // destroyed (C++ members are destroyed in reverse declaration order).
    aria::Property<std::size_t> itemsRevision_{0};
    aria::Subscription items_sub_;

public:
    aria::Computed<double> subtotal;
    aria::Computed<double> tax;
    aria::Computed<double> total;
    aria::Computed<int>    itemCount;

    aria::Property<std::string> draftName{"Apple"};
    aria::Property<double>      draftPrice{3.5};

    // Labels (i18n, auto-refresh on language change).
    aria::Property<std::string> nameLabel;
    aria::Property<std::string> priceLabel;
    aria::Property<std::string> addLabel;
    aria::Property<std::string> countLabel;
    aria::Property<std::string> subtotalLabel;
    aria::Property<std::string> taxLabel;
    aria::Property<std::string> totalLabel;
    aria::Property<std::string> checkoutLabel;

    /// Add item — also publishes ItemAddedToCart on the EventBus.
    aria::Command<> addItem;

    /// Checkout — publishes OrderPlaced on the EventBus.
    aria::Command<> checkout;

    // ── INavigationTarget (cross-module navigation) ────────────────────
    /// Strongly-typed channel: caller pushed CartArgs; consume the struct
    /// directly — compile-time checked fields, no json round-trip. Returns
    /// true to report the payload was consumed (enables Push success).
    bool on_navigate(const wb::module_api::CartArgs& args) override {
        draftName.set(args.product);
        draftPrice.set(args.price);
        return true;
    }

    /// Generic json channel: caller pushed a raw json object; parse freely —
    /// the payload may carry more fields than this VM needs. Returns true to
    /// report the payload was consumed.
    bool on_navigate(const nlohmann::json& payload) override {
        draftName.set(payload.value(ICartPage::kParamProduct, std::string{}));
        draftPrice.set(payload.value(ICartPage::kParamPrice, 0.0));
        return true;
    }

private:
    [[nodiscard]] double compute_subtotal_() const;
    [[nodiscard]] int compute_item_count_() const;
};

}  // namespace wb::cart
