#pragma once
//
// SyncView — Qt view for the "sync" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_sync_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::sync { class SyncVm; }

namespace wb::sync::qtview {

QWidget* build_view(SyncVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::sync::qtview
