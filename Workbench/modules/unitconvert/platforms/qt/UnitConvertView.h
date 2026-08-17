#pragma once
//
// UnitConvertView — Qt view for the "unitconvert" module (Aria free-function view).
//
// Uses the HostVm pattern: build_view() receives the UnitConvertVmHostVm
// (so it can read host.desc) and derives the inner UnitConvertVm from it.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::unitconvert { class UnitConvertVmHostVm; }

namespace wb::unitconvert::qtview {

QWidget* build_view(UnitConvertVmHostVm& host, aria::binding::BindingEngine& be);

}  // namespace wb::unitconvert::qtview
