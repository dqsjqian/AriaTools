#pragma once
//
// CartView — Qt view for the "cart" module.
//
// Owns the add-item form, the item list, the +/-/remove action buttons
// and the summary area. register_cart_view() is a thin entry point that
// constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::cart { class CartVm; }

namespace wb::cart::qtview {

class CartView {
public:
    CartView(CartVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::cart::qtview
