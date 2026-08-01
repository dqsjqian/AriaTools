#pragma once
//
// NotesModel — The module-level shared business state center for the notes module (not a single View's VM).
//
// Responsibilities:
//   * Holds the notes list (ObservableList) and the editable draft state of the "currently selected note";
//   * Performs load/save/delete/attachment-import of Markdown files via INotesService;
//   * On successful save, publishes the strongly-typed wb::events::NoteSaved via EventBus (subscribable by sync and other modules).
//
// Multiple VMs within the module may share the same NotesModel; all dependencies are injected via the constructor.
//
#include "models/Note.h"
#include "services/INotesService.h"

#include "aria/aria.hpp"
#include "aria/runtime/event_bus.hpp"

#include <memory>
#include <string>

namespace wb::notes {

class NotesModel {
public:
    NotesModel(std::shared_ptr<INotesService> service,
               aria::runtime::EventBus& bus)
        : service_(std::move(service)), bus_(bus) {}

    // List and selection/edit-draft state: VMs bind directly to these reactive members.
    aria::ObservableList<Note> notes;
    aria::Property<std::string> selectedId{""};
    aria::Property<std::string> draftTitle{""};
    aria::Property<std::string> draftBody{""};
    aria::Property<bool>        hasSelection{false};
    aria::Property<bool>        dirty{false};
    aria::Property<std::string> lastError{""};

    // Reload all notes from disk and clear the current selection.
    [[nodiscard]] bool reload();

    // Create a new empty note (persisted immediately, becomes the selected item).
    [[nodiscard]] bool create_note();

    // Select a note by id and load its content into the draft; an empty id clears the selection.
    void select(const NoteId& id);

    // Edit the draft (in-memory only, marks dirty; not persisted until save).
    void set_title(const std::string& title);
    void set_body(const std::string& body);

    // Save the current draft to disk and publish NoteSaved.
    [[nodiscard]] bool save_current();

    // Delete the currently selected note (along with its attachment directory) and clear the selection.
    [[nodiscard]] bool delete_current();

    // Import an attachment for the currently selected note; returns a relative Markdown path that can be inserted into the body.
    [[nodiscard]] bool import_attachment(const std::string& sourcePath,
                                         std::string& outRelativePath);

private:
    void apply_selection_(const Note* note);
    [[nodiscard]] std::shared_ptr<Note> find_(const NoteId& id) const;
    void set_error_(const NotesError error, std::string message);

    std::shared_ptr<INotesService> service_;
    aria::runtime::EventBus& bus_;
};

}  // namespace wb::notes
