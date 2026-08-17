#pragma once
//
// CartVm — Tab 3: shopping cart
//
// Scenario
//   Add / remove / change quantity → live aggregation of subtotal /
//   tax / total plus a "N items" badge.
//

#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "aria/command.hpp"
#include "aria/observable_list.hpp"
#include "aria/binding/view_model.hpp"

#include "models/CartItem.h"

#include <string>

namespace wb::cart {

class CartVm : public wb::core::BaseVm {
public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    static constexpr double kTaxRate = 0.08;

    CartVm();

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

    aria::Command<> addItem;

    void on_activate() override;
    void on_deactivate() override;

private:
    void recompute_();
};

}  // namespace wb::cart
