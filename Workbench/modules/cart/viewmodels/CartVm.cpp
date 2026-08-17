#include "viewmodels/CartVm.h"

#include "events/CrossModuleEvents.h"

#include <memory>

namespace wb::cart {

CartVm::CartVm(aria::runtime::EventBus& bus)
    : bus_(bus),
      subtotal([this] {
          (void)itemsRevision_.get();
          return compute_subtotal_();
      }),
      tax([this] { return subtotal.get() * kTaxRate; }),
      total([this] { return subtotal.get() + tax.get(); }),
      itemCount([this] {
          (void)itemsRevision_.get();
          return compute_item_count_();
      }),
      addItem(
          [this] {
              const auto name = draftName.get();
              if (name.empty() || draftPrice.get() <= 0) return;
              auto item = std::make_shared<CartItem>(
                  name, draftPrice.get(), 1);

              // The forwarding subscription belongs to CartItem, so removing
              // the item disconnects it automatically once the item dies.
              item->forward_qty_changes(
                  [bus = &bus_, name](const int newQty) {
                      // Property::on_changed exposes only the new value.
                      bus->publish(wb::shared::events::ItemQtyChanged{
                          name, 0, newQty});
                  });

              items.push_back(item);
              // Cross-module: notify other modules that an item was added.
              bus_.publish(wb::shared::events::ItemAddedToCart{
                  name, draftPrice.get(), 1});
          },
          [this] {
              return !draftName.get().empty() && draftPrice.get() > 0;
          }),
      checkout(
          [this] {
              if (items.empty()) return;
              // Cross-module: notify that an order was placed.
              bus_.publish(wb::shared::events::OrderPlaced{
                  "order-" + std::to_string(itemCount.get()),
                  total.get(),
                  itemCount.get()});
              // A reset is one cart mutation and therefore produces one
              // CartStateChanged snapshot with itemCount == 0.
              items.clear();
          },
          [this] {
              (void)itemsRevision_.get();
              return !items.empty();
          })
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

    // ObservableList is not a reactive graph node. Bridge every structural
    // or bubbled item change into Computed with a monotonic Property revision.
    items_sub_ = items.on_any_change([this] {
        itemsRevision_.set(itemsRevision_.get() + 1);
        bus_.publish(wb::shared::events::CartStateChanged{itemCount.get()});
    });
}

double CartVm::compute_subtotal_() const {
    double value = 0.0;
    for (const auto& item : items.snapshot()) {
        value += item->subtotal();
    }
    return value;
}

int CartVm::compute_item_count_() const {
    int value = 0;
    for (const auto& item : items.snapshot()) {
        value += item->qty_value();
    }
    return value;
}

}  // namespace wb::cart
