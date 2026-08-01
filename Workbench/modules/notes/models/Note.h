#pragma once
//
// Note — 一条笔记的纯业务数据。它不是 MVVM Model；NotesModel 才是模块级共享状态中心。
// 一条笔记对应 notes/<id>.md，正文中的图片使用相对路径，便于 Git 同步。
//
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace wb::notes {

using NoteId = std::string;

struct Note {
    NoteId id;                 ///< UUID（= 文件名）
    std::string title;
    std::string body;          ///< Markdown 正文（图文）
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
