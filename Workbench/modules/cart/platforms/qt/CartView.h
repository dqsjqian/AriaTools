#pragma once
//
// CartView — Qt view for the "cart" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_cart_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::cart { class CartVm; }

namespace wb::cart::qtview {

QWidget* build_view(CartVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::cart::qtview
