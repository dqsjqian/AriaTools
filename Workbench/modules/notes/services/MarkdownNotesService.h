#pragma once

#include "infra/storage/IStorageService.h"
#include "services/INotesService.h"

namespace wb::notes {

class MarkdownNotesService final : public INotesService {
public:
    explicit MarkdownNotesService(wb::services::IStorageService& storage)
        : storage_(storage) {}

    NotesResult<NoteList> load_all() override;
    NotesResult<Note> load(const NoteId& id) override;
    NotesResult<void> save(const Note& note) override;
    NotesResult<void> remove(const NoteId& id) override;
    NotesResult<std::string> import_attachment(
        const NoteId& noteId, const std::string& sourcePath) override;

private:
    wb::services::IStorageService& storage_;
};

}  // namespace wb::notes
