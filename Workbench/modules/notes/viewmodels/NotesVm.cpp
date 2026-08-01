#include "viewmodels/NotesVm.h"

#include "infra/i18n/I18n.h"

namespace wb::notes {

NotesVm::NotesVm(std::shared_ptr<NotesModel> model)
    : notes(model->notes),
      selectedId(model->selectedId),
      editTitle(""),
      editBody(""),
      hasSelection(model->hasSelection),
      dirty(model->dirty),
      status(""),
      addNote([this] {
          (void)model_->create_note();
          load_selection_into_editor_();
          refresh_status_();
      }),
      selectNote([this](const std::string& id) {
          model_->select(id);
          load_selection_into_editor_();
      }),
      saveNote([this] {
          model_->set_title(editTitle.get());
          model_->set_body(editBody.get());
          (void)model_->save_current();
      }),
      deleteSelected([this] {
          (void)model_->delete_current();
          load_selection_into_editor_();
          refresh_status_();
      }),
      model_(std::move(model))
{
    // Editor -> Model draft (not persisted until save, only marks dirty).
    track(editTitle.on_changed([this](const std::string& v) { model_->set_title(v); }));
    track(editBody.on_changed([this](const std::string& v) { model_->set_body(v); }));

    // Static text: written near the use site, auto-refreshed on language switch.
    text(title,            "title");
    text(hint,             "hint");
    text(addLabel,         "add");
    text(saveLabel,        "save");
    text(deleteLabel,      "delete");
    text(titlePlaceholder, "title_placeholder");
    text(bodyPlaceholder,  "body_placeholder");

    // Dynamic status: refreshes both on list-size change and on language change.
    localize([this] { refresh_status_(); });
    track(notes.on_any_change([this]() { refresh_status_(); }));

    load_selection_into_editor_();
}

void NotesVm::on_activate() {
    (void)model_->reload();
    load_selection_into_editor_();
    refresh_status_();
}

void NotesVm::on_deactivate() { bag().clear(); }

void NotesVm::load_selection_into_editor_() {
    // Sync from the Model draft to the editor Properties; this only mirrors, it does not write back to the Model.
    editTitle.set(model_->draftTitle.get());
    editBody.set(model_->draftBody.get());
}

void NotesVm::refresh_status_() {
    const auto n = notes.size();
    if (n == 0) {
        status.set(wb::i18n::str("empty"));
    } else {
        status.set(wb::i18n::str("count_prefix") + std::to_string(n) + wb::i18n::str("count_suffix"));
    }
}

}  // namespace wb::notes
