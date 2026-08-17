#pragma once
//
// UnitConvertView — Qt view for the "unitconvert" module.
//
// Binds directly to UnitConvertVm.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::unitconvert { class UnitConvertVm; }

namespace wb::unitconvert::qtview {

class UnitConvertView {
public:
    UnitConvertView(UnitConvertVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::unitconvert::qtview
