#pragma once
//
// ThemeView — Qt view for the "theme" module.
//
// Uses the HostVm pattern: the constructor receives ThemeVmHostVm and
// derives the inner ThemeVm from it (for currentDisplayName, available_themes,
// theme(), pick()).
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

class QComboBox;
class QFrame;
class QLabel;

namespace wb::theme { class ThemeVmHostVm; }

namespace wb::theme::qtview {

class ThemeView {
public:
    ThemeView(ThemeVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::theme::qtview
