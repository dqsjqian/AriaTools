#pragma once
//
// TipCalcView — Qt view for the "tipcalc" module.
//
// Owns the bill/tip/people form, the result rows, and the round-up
// button. register_tipcalc_view() is a thin entry point that constructs
// the view and returns its root widget.
//
#include "support/UiHelpers.h"
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
