#pragma once
//
// TipCalcView — Qt view for the "tipcalc" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::tipcalc { class TipCalcVm; }

namespace wb::tipcalc::qtview {

class TipCalcView {
public:
    TipCalcView(TipCalcVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::tipcalc::qtview
