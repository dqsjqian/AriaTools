#pragma once
//
// SyncView — Qt view for the "sync" module.
//
// Owns the remote-repo config form, sync action buttons, status label,
// and read-only log view. register_sync_view() is a thin entry point
// that constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
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
