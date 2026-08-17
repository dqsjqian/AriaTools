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
              auto item = std::make_shared<CartItem>(
                  name, draftPrice.get(), 1);

              // ── Model → EventBus: subscribe to this item's qty changes ──
              // When the user changes the quantity (via the View's +/- buttons),
              // the CartItem model's qty Property fires, and we forward it as
              // an ItemQtyChanged event. Multiple modules' VMs receive it:
              //   - dashboard VM updates the cart badge count
              //   - chat VM posts a system message
              //   - notes VM creates an operation-log entry
              qty_subs_.push_back(item->qty().on_changed(
                  [this, name](const int /*newQty*/) {
                      // We don't track oldQty here (Property::on_changed
                      // only gives the new value); pass 0 as oldQty.
                      bus_.publish(wb::shared::events::ItemQtyChanged{
                          name, 0, 0});
                  }));

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
              // Drop the qty subscriptions (items are gone).
              qty_subs_.clear();
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
    // qty_subs_ are NOT in bag() — they persist across tab switches so the
    // model→event forwarding keeps working even when the cart tab is inactive.
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
