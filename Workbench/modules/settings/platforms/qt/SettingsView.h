#pragma once
//
// SettingsView — Qt view for the "settings" module.
//
// Owns the language selector + form rows. register_settings_view() is a
// thin entry point that constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

class QComboBox;
class QLabel;

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
