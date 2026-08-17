#include "viewmodels/CartVm.h"

#include "events/CrossModuleEvents.h"

#include <memory>

namespace wb::cart {

CartVm::CartVm(aria::runtime::EventBus& bus)
    : bus_(bus),
      addItem(
          [this] {
              const auto name = draftName.get();
              if (name.empty() || draftPrice.get() <= 0) return;
              items.push_back(std::make_shared<CartItem>(
                  name, draftPrice.get(), 1));
              // Cross-module: notify other modules that an item was added.
              bus_.publish(wb::shared::events::ItemAddedToCart{
                  name, draftPrice.get(), 1});
          },
          [this] {
              return !draftName.get().empty() && draftPrice.get() > 0;
          }),
      checkout(
          [this] {
              if (items.snapshot().empty()) return;
              // Cross-module: notify that an order was placed.
              bus_.publish(wb::shared::events::OrderPlaced{
                  "order-" + std::to_string(itemCount.get()),
                  total.get(),
                  itemCount.get()});
              // Clear the cart after checkout.
              while (!items.snapshot().empty()) {
                  items.remove_at(0);
              }
          },
          [this] { return !items.snapshot().empty(); })
{
    text(title,         "title");
    text(desc,          "desc");
    text(nameLabel,     "name_label");
    text(priceLabel,    "price_label");
    text(addLabel,      "add");
    text(countLabel,    "count");
    text(subtotalLabel, "subtotal");
    text(taxLabel,      "tax");
    text(totalLabel,    "total");
    text(checkoutLabel, "checkout");
}

void CartVm::on_activate() {
    bag() += items.on_any_change([this] { recompute_(); });
    recompute_();
}

void CartVm::on_deactivate() {
    bag().clear();
}

void CartVm::recompute_() {
    double s = 0.0;
    int    n = 0;
    for (auto& item : items.snapshot()) {
        s += item->subtotal();
        n += item->qty_value();
    }
    aria::batch([this, s, n] {
        subtotal  = s;
        tax       = s * kTaxRate;
        total     = s + s * kTaxRate;
        itemCount = n;
    });
}

}  // namespace wb::cart
