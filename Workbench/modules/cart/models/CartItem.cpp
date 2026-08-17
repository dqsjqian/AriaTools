#include "models/CartItem.h"

#include <utility>

namespace wb::cart {

CartItem::CartItem(std::string name, double price, int qty)
    : name_(std::move(name)), price_(price), qty_(qty) {}

aria::Subscription
CartItem::on_changed(std::function<void(const CartItem&)> fn) {
    return qty_.on_changed([this, fn = std::move(fn)](int) { fn(*this); });
}

void CartItem::forward_qty_changes(std::function<void(int)> fn) {
    qty_forward_sub_ = qty_.on_changed(std::move(fn));
}

}  // namespace wb::cart
