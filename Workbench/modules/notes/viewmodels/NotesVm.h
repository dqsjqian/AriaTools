#pragma once
//
// NotesVm — The notes view model. Only maintains page-level reactive state and Commands;
// list/selection/edit/save/delete and other business logic is fully delegated to NotesModel. The View only binds to the VM.
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "models/NotesModel.h"

#include <memory>

namespace wb::notes {

class NotesVm final : public wb::core::BaseVm {
public:
    explicit NotesVm(std::shared_ptr<NotesModel> model);

    // List and edit state (forwarded from the Model, for the View to bind).
    aria::ObservableList<Note>& notes;
    aria::Property<std::string>& selectedId;
    aria::Property<std::string>  editTitle;
    aria::Property<std::string>  editBody;
    aria::Property<bool>&        hasSelection;
    aria::Property<bool>&        dirty;
    aria::Property<std::string>  status;   ///< Dynamic: recomputed on language + list size

    // Commands
    aria::Command<>            addNote;
    aria::Command<std::string> selectNote;   ///< Parameter: note id
    aria::Command<>            saveNote;
    aria::Command<>            deleteSelected;

    // UI text (updates on language change)
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> addLabel;
    aria::Property<std::string> saveLabel;
    aria::Property<std::string> deleteLabel;
    aria::Property<std::string> titlePlaceholder;
    aria::Property<std::string> bodyPlaceholder;

    void on_activate() override;
    void on_deactivate() override;

private:
    void refresh_status_();
    void load_selection_into_editor_();

    std::shared_ptr<NotesModel> model_;
};

}  // namespace wb::notes
