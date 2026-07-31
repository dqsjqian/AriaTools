#include "viewmodels/NotesVm.h"

namespace wb::notes {

NotesVm::NotesVm(wb::services::II18nService& i18n)
    : wb::core::LocalizedVm(i18n, "notes"),
      addNote([this] {
          auto note = std::make_shared<NoteModel>();
          note->id    = "note-" + std::to_string(++counter_);
          note->title = tr("new_title") + " " + std::to_string(counter_);
          note->body  = "";
          notes.push_back(std::move(note));
          refresh_status_();
      }),
      deleteSelected([this] {
          if (notes.size() > 0) {
              notes.remove_at(notes.size() - 1);
              refresh_status_();
          }
      })
{
    bind_text(title, "title");
    bind_text(hint, "hint");
    bind_text(addLabel, "add");
    bind_text(deleteLabel, "delete");
    // status 是动态文案：语言变化时也要重算（含数量）。
    track(i18n.language().on_changed([this](const std::string&) { refresh_status_(); }));
    refresh_status_();
}

void NotesVm::on_activate() { refresh_status_(); }
void NotesVm::on_deactivate() { bag().clear(); }

void NotesVm::refresh_status_() {
    const auto n = notes.size();
    if (n == 0) {
        status.set(tr("empty"));
    } else {
        status.set(tr("count_prefix") + std::to_string(n) + tr("count_suffix"));
    }
}

}  // namespace wb::notes
