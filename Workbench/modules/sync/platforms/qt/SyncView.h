#pragma once
//
// SyncView — Qt view for the "sync" module.
//
// The constructor creates the widgets, wires the bindings, and the root widget
// is exposed via widget().
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::sync { class SyncVm; }

namespace wb::sync::qtview {

class SyncView {
public:
    SyncView(SyncVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::sync::qtview
