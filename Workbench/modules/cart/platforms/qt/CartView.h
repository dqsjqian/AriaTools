#pragma once
//
// CartView — Qt view for the "cart" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
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
