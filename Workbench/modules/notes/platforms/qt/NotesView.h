#pragma once
//
// NotesView — Qt view for the "notes" module (Aria free-function view).
//
// build_view() creates the widgets, wires the bindings, and returns the root
// widget. register_notes_view() registers it with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::notes { class NotesVm; }

namespace wb::notes::qtview {

QWidget* build_view(NotesVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::notes::qtview
