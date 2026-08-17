#pragma once
//
// NotesView — Qt view for the "notes" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::notes { class NotesVm; }

namespace wb::notes::qtview {

class NotesView {
public:
    NotesView(NotesVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::notes::qtview
