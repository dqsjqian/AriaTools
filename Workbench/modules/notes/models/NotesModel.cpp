#include "models/NotesModel.h"

#include "module_api/AppEvents.h"
#include "infra/log/Log.h"

#include <chrono>
#include <random>

namespace wb::notes {

namespace {

std::int64_t now_seconds() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

// 生成文件系统安全的唯一 id：时间戳(36 进制) + 随机后缀，仅含 [0-9a-z]。
NoteId make_note_id() {
    static constexpr char kAlphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    auto to_base36 = [](std::uint64_t value) {
        if (value == 0) return std::string{"0"};
        std::string out;
        while (value > 0) {
            out.push_back(kAlphabet[value % 36]);
            value /= 36;
        }
        return std::string(out.rbegin(), out.rend());
    };

    const auto stamp = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<int> distribution(0, 35);

    std::string suffix;
    suffix.reserve(6);
    for (int i = 0; i < 6; ++i) suffix.push_back(kAlphabet[distribution(generator)]);

    return "note-" + to_base36(stamp) + "-" + suffix;
}

}  // namespace

std::shared_ptr<Note> NotesModel::find_(const NoteId& id) const {
    if (id.empty()) return nullptr;
    for (auto& note : notes.snapshot()) {
        if (note && note->id == id) return note;
    }
    return nullptr;
}

void NotesModel::set_error_(const NotesError error, std::string message) {
    lastError.set(error == NotesError::None ? std::string{} : std::move(message));
}

void NotesModel::apply_selection_(const Note* note) {
    if (note == nullptr) {
        selectedId.set("");
        draftTitle.set("");
        draftBody.set("");
        hasSelection.set(false);
    } else {
        selectedId.set(note->id);
        draftTitle.set(note->title);
        draftBody.set(note->body);
        hasSelection.set(true);
    }
    dirty.set(false);
}

bool NotesModel::reload() {
    auto result = service_->load_all();
    if (!result.ok()) {
        set_error_(result.error, result.message);
        log_error << "reload failed: " << result.message;
        return false;
    }
    set_error_(NotesError::None, {});
    notes.clear();
    for (auto& note : result.value) {
        notes.push_back(std::make_shared<Note>(std::move(note)));
    }
    apply_selection_(nullptr);
    log_info << "reloaded " << notes.size() << " notes";
    return true;
}

bool NotesModel::create_note() {
    Note note;
    note.id = make_note_id();
    note.title = {};
    note.body = {};
    note.updatedAt = now_seconds();

    auto saved = service_->save(note);
    if (!saved.ok()) {
        set_error_(saved.error, saved.message);
        log_error << "create failed: " << saved.message;
        return false;
    }
    set_error_(NotesError::None, {});

    // 新笔记更新时间最大，插到列表头，成为选中项。
    auto shared = std::make_shared<Note>(note);
    notes.insert(0, shared);
    apply_selection_(shared.get());
    bus_.publish(wb::events::NoteSaved{note.id});
    log_info << "created note " << note.id;
    return true;
}

void NotesModel::select(const NoteId& id) {
    apply_selection_(find_(id).get());
}

void NotesModel::set_title(const std::string& title) {
    if (!hasSelection.get()) return;
    if (draftTitle.get() == title) return;
    draftTitle.set(title);
    dirty.set(true);
}

void NotesModel::set_body(const std::string& body) {
    if (!hasSelection.get()) return;
    if (draftBody.get() == body) return;
    draftBody.set(body);
    dirty.set(true);
}

bool NotesModel::save_current() {
    if (!hasSelection.get()) return false;
    const NoteId id = selectedId.get();
    auto existing = find_(id);
    if (!existing) {
        set_error_(NotesError::NotFound, "selected note no longer exists");
        return false;
    }

    Note updated = *existing;
    updated.title = draftTitle.get();
    updated.body = draftBody.get();
    updated.updatedAt = now_seconds();
    auto saved = service_->save(updated);
    if (!saved.ok()) {
        set_error_(saved.error, saved.message);
        log_error << "save failed for " << id << ": " << saved.message;
        return false;
    }
    set_error_(NotesError::None, {});

    // 用 replace_at 就地替换列表项：Note 是纯数据结构、没有自身变更信号，
    // 直接写 *existing 不会触发 ObservableList 通知，列表标题不会刷新。
    // replace_at 会发出 Replace 事件，驱动 View 重建列表显示最新标题。
    for (std::size_t i = 0; i < notes.size(); ++i) {
        if (auto item = notes.at(i); item && item->id == id) {
            notes.replace_at(i, std::make_shared<Note>(updated));
            break;
        }
    }
    dirty.set(false);
    bus_.publish(wb::events::NoteSaved{updated.id});
    log_info << "saved note " << updated.id << " (title=\"" << updated.title << "\")";
    return true;
}

bool NotesModel::delete_current() {
    if (!hasSelection.get()) return false;
    const NoteId id = selectedId.get();

    auto removed = service_->remove(id);
    if (!removed.ok()) {
        set_error_(removed.error, removed.message);
        log_error << "delete failed for " << id << ": " << removed.message;
        return false;
    }
    set_error_(NotesError::None, {});

    for (std::size_t i = 0; i < notes.size(); ++i) {
        auto note = notes.at(i);
        if (note && note->id == id) {
            notes.remove_at(i);
            break;
        }
    }
    apply_selection_(nullptr);
    log_info << "deleted note " << id;
    return true;
}

bool NotesModel::import_attachment(const std::string& sourcePath,
                                   std::string& outRelativePath) {
    if (!hasSelection.get()) {
        set_error_(NotesError::InvalidData, "no note selected for attachment");
        return false;
    }
    auto result = service_->import_attachment(selectedId.get(), sourcePath);
    if (!result.ok()) {
        set_error_(result.error, result.message);
        return false;
    }
    set_error_(NotesError::None, {});
    outRelativePath = result.value;
    return true;
}

}  // namespace wb::notes
