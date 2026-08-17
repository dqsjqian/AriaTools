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
#include "aria/command.hpp"
#include "aria/observable_list.hpp"
#include "aria/binding/view_model.hpp"
#include "aria/runtime/event_bus.hpp"

#include "models/CartItem.h"

#include <string>

namespace wb::cart {

class CartVm : public wb::core::BaseVm {
public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    static constexpr double kTaxRate = 0.08;

    /// Construct with the shared EventBus (for cross-module publishing).
    explicit CartVm(aria::runtime::EventBus& bus);

    aria::ObservableList<CartItem> items;

    aria::Property<double> subtotal{0.0};
    aria::Property<double> tax{0.0};
    aria::Property<double> total{0.0};
    aria::Property<int>    itemCount{0};

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

    void on_activate() override;
    void on_deactivate() override;

private:
    aria::runtime::EventBus& bus_;
    void recompute_();
};

}  // namespace wb::cart
