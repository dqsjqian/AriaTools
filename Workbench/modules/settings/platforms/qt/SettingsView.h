#pragma once
//
// SettingsView — Qt view for the "settings" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_settings_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::settings { class SettingsVm; }

namespace wb::settings::qtview {

QWidget* build_view(SettingsVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::settings::qtview
