#pragma once

#include "models/Note.h"

#include <string>

namespace wb::notes {

class INotesService {
public:
    virtual ~INotesService() = default;

    [[nodiscard]] virtual NotesResult<NoteList> load_all() = 0;
    [[nodiscard]] virtual NotesResult<Note> load(const NoteId& id) = 0;
    [[nodiscard]] virtual NotesResult<void> save(const Note& note) = 0;
    [[nodiscard]] virtual NotesResult<void> remove(const NoteId& id) = 0;

    // Copy sourcePath into notes/assets/<noteId>/ and return a relative Markdown path.
    [[nodiscard]] virtual NotesResult<std::string>
        import_attachment(const NoteId& noteId, const std::string& sourcePath) = 0;
};

}  // namespace wb::notes
