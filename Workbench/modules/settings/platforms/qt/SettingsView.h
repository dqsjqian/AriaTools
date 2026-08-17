#pragma once
//
// SettingsView — Qt view for the "settings" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::settings { class SettingsVm; }

namespace wb::settings::qtview {

class SettingsView {
public:
    SettingsView(SettingsVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::settings::qtview
