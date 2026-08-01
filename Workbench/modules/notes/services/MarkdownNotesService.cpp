#include "services/MarkdownNotesService.h"

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

namespace wb::notes {

namespace {

constexpr std::string_view kNotesDir = "notes";
constexpr std::string_view kFrontMatter = "---\n";

bool valid_id(std::string_view id) {
    if (id.empty()) return false;
    return std::all_of(id.begin(), id.end(), [](unsigned char ch) {
        return std::isalnum(ch) || ch == '-' || ch == '_';
    });
}

std::string quote(std::string_view value) {
    std::string result{"\""};
    result.reserve(value.size() + 2);
    for (char ch : value) {
        switch (ch) {
            case '\\': result += "\\\\"; break;
            case '"':  result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += ch; break;
        }
    }
    result += '"';
    return result;
}

bool unquote(std::string_view value, std::string& result) {
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        return false;
    }
    result.clear();
    for (std::size_t i = 1; i + 1 < value.size(); ++i) {
        char ch = value[i];
        if (ch != '\\') {
            result += ch;
            continue;
        }
        if (++i + 1 > value.size()) return false;
        switch (value[i]) {
            case '\\': result += '\\'; break;
            case '"':  result += '"'; break;
            case 'n':  result += '\n'; break;
            case 'r':  result += '\r'; break;
            case 't':  result += '\t'; break;
            default: return false;
        }
    }
    return true;
}

std::string serialize(const Note& note) {
    std::ostringstream output;
    output << kFrontMatter
           << "id: " << quote(note.id) << '\n'
           << "title: " << quote(note.title) << '\n'
           << "updated_at: " << note.updatedAt << '\n'
           << kFrontMatter
           << note.body;
    return output.str();
}

NotesResult<Note> parse(std::string_view content) {
    if (!content.starts_with(kFrontMatter)) {
        return NotesResult<Note>::failure(
            NotesError::InvalidData, "missing Markdown front matter");
    }

    const auto close = content.find(kFrontMatter, kFrontMatter.size());
    if (close == std::string_view::npos) {
        return NotesResult<Note>::failure(
            NotesError::InvalidData, "unterminated Markdown front matter");
    }

    Note note;
    std::istringstream metadata{std::string(content.substr(
        kFrontMatter.size(), close - kFrontMatter.size()))};
    std::string line;
    while (std::getline(metadata, line)) {
        const auto separator = line.find(": ");
        if (separator == std::string::npos) continue;
        const std::string_view key{line.data(), separator};
        const std::string_view value{line.data() + separator + 2,
                                     line.size() - separator - 2};
        if (key == "id") {
            if (!unquote(value, note.id)) {
                return NotesResult<Note>::failure(
                    NotesError::InvalidData, "invalid note id");
            }
        } else if (key == "title") {
            if (!unquote(value, note.title)) {
                return NotesResult<Note>::failure(
                    NotesError::InvalidData, "invalid note title");
            }
        } else if (key == "updated_at") {
            const auto [end, error] = std::from_chars(
                value.data(), value.data() + value.size(), note.updatedAt);
            if (error != std::errc{} || end != value.data() + value.size()) {
                return NotesResult<Note>::failure(
                    NotesError::InvalidData, "invalid note timestamp");
            }
        }
    }

    if (!valid_id(note.id)) {
        return NotesResult<Note>::failure(
            NotesError::InvalidData, "missing or unsafe note id");
    }
    note.body = std::string(content.substr(close + kFrontMatter.size()));
    return NotesResult<Note>::success(std::move(note));
}

std::string note_path(const NoteId& id) {
    return std::string(kNotesDir) + "/" + id + ".md";
}

}  // namespace

NotesResult<NoteList> MarkdownNotesService::load_all() {
    storage_.ensure_dir(std::string(kNotesDir));
    NoteList notes;
    for (const auto& file : storage_.list(std::string(kNotesDir))) {
        if (!file.ends_with(".md")) continue;
        auto content = storage_.read_text(std::string(kNotesDir) + "/" + file);
        if (!content) {
            return NotesResult<NoteList>::failure(
                NotesError::ReadFailed, "failed to read " + file);
        }
        auto parsed = parse(*content);
        if (!parsed.ok()) {
            return NotesResult<NoteList>::failure(
                parsed.error, file + ": " + parsed.message);
        }
        if (file != parsed.value.id + ".md") {
            return NotesResult<NoteList>::failure(
                NotesError::InvalidData, file + ": id does not match filename");
        }
        notes.push_back(std::move(parsed.value));
    }
    std::sort(notes.begin(), notes.end(), [](const Note& lhs, const Note& rhs) {
        if (lhs.updatedAt != rhs.updatedAt) return lhs.updatedAt > rhs.updatedAt;
        return lhs.id < rhs.id;
    });
    return NotesResult<NoteList>::success(std::move(notes));
}

NotesResult<Note> MarkdownNotesService::load(const NoteId& id) {
    if (!valid_id(id)) {
        return NotesResult<Note>::failure(NotesError::InvalidData, "unsafe note id");
    }
    auto content = storage_.read_text(note_path(id));
    if (!content) {
        return NotesResult<Note>::failure(NotesError::NotFound, "note not found");
    }
    auto result = parse(*content);
    if (result.ok() && result.value.id != id) {
        return NotesResult<Note>::failure(
            NotesError::InvalidData, "note id does not match filename");
    }
    return result;
}

NotesResult<void> MarkdownNotesService::save(const Note& note) {
    if (!valid_id(note.id)) {
        return NotesResult<void>::failure(NotesError::InvalidData, "unsafe note id");
    }
    storage_.ensure_dir(std::string(kNotesDir));
    if (!storage_.write_text(note_path(note.id), serialize(note))) {
        return NotesResult<void>::failure(NotesError::WriteFailed, "failed to save note");
    }
    return NotesResult<void>::success();
}

NotesResult<void> MarkdownNotesService::remove(const NoteId& id) {
    if (!valid_id(id)) {
        return NotesResult<void>::failure(NotesError::InvalidData, "unsafe note id");
    }
    if (!storage_.remove(note_path(id))) {
        return NotesResult<void>::failure(NotesError::NotFound, "note not found");
    }

    std::error_code error;
    fs::remove_all(fs::path(storage_.data_dir()) / kNotesDir / "assets" / id, error);
    if (error) {
        return NotesResult<void>::failure(
            NotesError::RemoveFailed, "note removed but attachments could not be removed");
    }
    return NotesResult<void>::success();
}

NotesResult<std::string> MarkdownNotesService::import_attachment(
    const NoteId& noteId, const std::string& sourcePath) {
    if (!valid_id(noteId)) {
        return NotesResult<std::string>::failure(
            NotesError::InvalidData, "unsafe note id");
    }

    const fs::path source{sourcePath};
    std::error_code error;
    if (!fs::is_regular_file(source, error)) {
        return NotesResult<std::string>::failure(
            NotesError::ImportFailed, "attachment source is not a file");
    }

    const fs::path relativeDir = fs::path("notes") / "assets" / noteId;
    const fs::path destinationDir = fs::path(storage_.data_dir()) / relativeDir;
    fs::create_directories(destinationDir, error);
    if (error) {
        return NotesResult<std::string>::failure(
            NotesError::ImportFailed, "failed to create attachment directory");
    }

    fs::path destination = destinationDir / source.filename();
    for (int suffix = 2; fs::exists(destination, error); ++suffix) {
        destination = destinationDir /
            (source.stem().string() + "-" + std::to_string(suffix) + source.extension().string());
    }
    fs::copy_file(source, destination, fs::copy_options::none, error);
    if (error) {
        return NotesResult<std::string>::failure(
            NotesError::ImportFailed, "failed to copy attachment");
    }

    const fs::path markdownPath = fs::path("assets") / noteId / destination.filename();
    return NotesResult<std::string>::success(markdownPath.generic_string());
}

}  // namespace wb::notes
