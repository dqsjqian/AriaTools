#pragma once
//
// UnitConvertView — Qt view for the "unitconvert" module.
//
// Uses the HostVm pattern: the constructor receives the UnitConvertVmHostVm
// (so it can read host.desc) and derives the inner UnitConvertVm from it.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::unitconvert { class UnitConvertVmHostVm; }

namespace wb::unitconvert::qtview {

class UnitConvertView {
public:
    UnitConvertView(UnitConvertVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::unitconvert::qtview
