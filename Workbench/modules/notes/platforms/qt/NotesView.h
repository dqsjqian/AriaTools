#pragma once
//
// NotesView — Qt view for the "notes" module.
//
// Owns the note list, the editor area, and all subscriptions. The list
// stores the note id in each item's UserRole to avoid depending on row
// numbers.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

class QListWidget;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QLabel;

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
