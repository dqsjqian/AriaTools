#pragma once
//
// ThemeView — Qt view for the "theme" module (Aria free-function view).
//
// Uses the HostVm pattern: build_view() receives ThemeVmHostVm and derives
// the inner ThemeVm from it (for currentDisplayName, available_themes,
// theme(), pick()).
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::theme { class ThemeVmHostVm; }

namespace wb::theme::qtview {

QWidget* build_view(ThemeVmHostVm& host, aria::binding::BindingEngine& be);

}  // namespace wb::theme::qtview
