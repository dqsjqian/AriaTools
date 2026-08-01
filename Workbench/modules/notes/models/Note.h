#pragma once
//
// Note — Pure business data of a single note. This is not the MVVM Model; NotesModel is the module-level shared state center.
// Each note maps to notes/<id>.md; images in the body use relative paths for easy Git sync.
//
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace wb::notes {

using NoteId = std::string;

struct Note {
    NoteId id;                 ///< UUID (also the filename)
    std::string title;
    std::string body;          ///< Markdown body (text + images)
    std::int64_t updatedAt = 0;
};

struct NoteDraft {
    NoteId id;
    std::string title;
    std::string body;
};

enum class NotesError {
    None,
    NotFound,
    InvalidData,
    ReadFailed,
    WriteFailed,
    RemoveFailed,
    ImportFailed,
};

template<typename T>
struct NotesResult {
    T value{};
    NotesError error = NotesError::None;
    std::string message;

    [[nodiscard]] bool ok() const noexcept { return error == NotesError::None; }

    static NotesResult success(T value) {
        return {std::move(value), NotesError::None, {}};
    }

    static NotesResult failure(NotesError error, std::string message) {
        return {{}, error, std::move(message)};
    }
};

template<>
struct NotesResult<void> {
    NotesError error = NotesError::None;
    std::string message;

    [[nodiscard]] bool ok() const noexcept { return error == NotesError::None; }

    static NotesResult success() { return {}; }
    static NotesResult failure(NotesError error, std::string message) {
        return {error, std::move(message)};
    }
};

using NoteList = std::vector<Note>;

}  // namespace wb::notes
