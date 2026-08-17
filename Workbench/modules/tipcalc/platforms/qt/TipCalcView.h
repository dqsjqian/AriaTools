#pragma once
//
// TipCalcView — Qt view for the "tipcalc" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_tipcalc_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::tipcalc { class TipCalcVm; }

namespace wb::tipcalc::qtview {

QWidget* build_view(TipCalcVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::tipcalc::qtview
