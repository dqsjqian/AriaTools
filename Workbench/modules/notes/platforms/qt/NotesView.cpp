#include "NotesView.h"
#include "support/UiHelpers.h"
#include "viewmodels/NotesVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include <string>

namespace wb::notes::qtview {

// Store the note id in the list item's UserRole to avoid depending on row numbers.
static constexpr int kNoteIdRole = Qt::UserRole + 1;

NotesView::NotesView(NotesVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& subs = wb::ui::subs_attached_to(root_);
    auto* root = new QVBoxLayout(root_);

    auto* title = wb::ui::make_title("");
    auto* hint = wb::ui::make_info("");
    root->addWidget(title);
    root->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));

    auto* body = new QHBoxLayout;
    root->addLayout(body, 1);

    // Left column: list + status + create button.
    auto* leftCol = new QVBoxLayout;
    auto* list = new QListWidget;
    auto* statusLbl = new QLabel;
    auto* addBtn = new QPushButton;
    leftCol->addWidget(list, 1);
    leftCol->addWidget(statusLbl);
    leftCol->addWidget(addBtn);
    body->addLayout(leftCol, 1);
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
    be.bind_text_oneway(vm.addLabel, wb::ui::view_for(addBtn));
    be.bind_command(vm.addNote, wb::ui::view_for(addBtn));

    // Right column: title editor + body editor + save/delete.
    auto* rightCol = new QVBoxLayout;
    auto* titleEdit = new QLineEdit;
    auto* bodyEdit = new QPlainTextEdit;
    auto* actionRow = new QHBoxLayout;
    auto* saveBtn = new QPushButton;
    auto* delBtn = new QPushButton;
    actionRow->addWidget(saveBtn);
    actionRow->addWidget(delBtn);
    actionRow->addStretch();
    rightCol->addWidget(titleEdit);
    rightCol->addWidget(bodyEdit, 1);
    rightCol->addLayout(actionRow);
    body->addLayout(rightCol, 2);

    // Placeholder text updates on language change.
    auto applyPlaceholders = [titleEdit, bodyEdit, &vm]() {
        titleEdit->setPlaceholderText(QString::fromStdString(vm.titlePlaceholder.get()));
        bodyEdit->setPlaceholderText(QString::fromStdString(vm.bodyPlaceholder.get()));
    };
    applyPlaceholders();
    subs.push_back(vm.titlePlaceholder.on_changed(
        [applyPlaceholders](const std::string&) { applyPlaceholders(); }));
    subs.push_back(vm.bodyPlaceholder.on_changed(
        [applyPlaceholders](const std::string&) { applyPlaceholders(); }));

    // User input -> VM (bind_editable_text only does View->VM, to avoid cursor reset while typing).
    wb::ui::bind_editable_text(be, vm.editTitle, titleEdit);
    wb::ui::bind_editable_text(be, vm.editBody, bodyEdit);
    be.bind_text_oneway(vm.saveLabel, wb::ui::view_for(saveBtn));
    be.bind_text_oneway(vm.deleteLabel, wb::ui::view_for(delBtn));
    be.bind_command(vm.saveNote, wb::ui::view_for(saveBtn));
    be.bind_command(vm.deleteSelected, wb::ui::view_for(delBtn));

    // Editors are only enabled when there is a selection.
    auto applyEnabled = [titleEdit, bodyEdit, saveBtn, delBtn, &vm]() {
        const bool on = vm.hasSelection.get();
        titleEdit->setEnabled(on);
        bodyEdit->setEnabled(on);
        saveBtn->setEnabled(on);
        delBtn->setEnabled(on);
    };
    applyEnabled();
    subs.push_back(vm.hasSelection.on_changed(
        [applyEnabled](bool) { applyEnabled(); }));

    // VM -> View: push the VM editor Properties into the widgets. Only call setText when the
    // content actually differs, to avoid triggering a cursor reset while the user is typing
    // (when the widget already equals the new value).
    // Key point: subscribe to editTitle/editBody changes rather than selectedId — the VM only
    // updates these two Properties after model.select, so reading them here yields the target
    // note's content; if we subscribed to selectedId instead, the callback would fire before
    // the VM has refreshed the editor Properties and read stale text from the previous note.
    auto pushTitle = [titleEdit, &vm]() {
        const QString v = QString::fromStdString(vm.editTitle.get());
        if (titleEdit->text() == v) return;
        const QSignalBlocker b(titleEdit);
        titleEdit->setText(v);
    };
    auto pushBody = [bodyEdit, &vm]() {
        const QString v = QString::fromStdString(vm.editBody.get());
        if (bodyEdit->toPlainText() == v) return;
        const QSignalBlocker b(bodyEdit);
        bodyEdit->setPlainText(v);
    };
    pushTitle();
    pushBody();
    subs.push_back(vm.editTitle.on_changed([pushTitle](const std::string&) { pushTitle(); }));
    subs.push_back(vm.editBody.on_changed([pushBody](const std::string&) { pushBody(); }));

    // List rebuild: refresh on Model list change and keep the selection highlight.
    auto rebuild = [list, &vm]() {
        const QSignalBlocker blocker(list);
        list->clear();
        const std::string sel = vm.selectedId.get();
        for (std::size_t i = 0; i < vm.notes.size(); ++i) {
            auto n = vm.notes.at(i);
            if (!n) continue;
            auto* item = new QListWidgetItem(QString::fromStdString(vm.display_title(*n)));
            item->setData(kNoteIdRole, QString::fromStdString(n->id));
            list->addItem(item);
            if (n->id == sel) list->setCurrentItem(item);
        }
    };
    rebuild();
    subs.push_back(vm.notes.on_any_change([rebuild]() { rebuild(); }));
    subs.push_back(vm.selectedId.on_changed(
        [rebuild](const std::string&) { rebuild(); }));

    // List click -> notify the VM of the selection.
    QObject::connect(list, &QListWidget::currentItemChanged,
                     [&vm](QListWidgetItem* current, QListWidgetItem*) {
                         if (!current) return;
                         const std::string id =
                             current->data(kNoteIdRole).toString().toStdString();
                         if (id != vm.selectedId.get()) vm.selectNote.execute(id);
                     });
}

}  // namespace wb::notes::qtview
