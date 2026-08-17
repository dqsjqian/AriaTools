// ────────────────────────────────────────────────────────────────────────────
//  NotesJniBinding.cpp — see NotesJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/NotesJniBinding.h"

namespace wb::notes {

void subscribe_notes(aria::runtime::EventBus& bus, NotesVm& vm,
                     std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::push_property;

    bind_str(subs, "notes", "title", vm.title);
    bind_str(subs, "notes", "hint",  vm.hint);
    bind_str(subs, "notes", "status", vm.status);
    bind_str(subs, "notes", "editTitle", vm.editTitle);
    bind_str(subs, "notes", "editBody",  vm.editBody);
    bind_str(subs, "notes", "add",   vm.addLabel);
    bind_str(subs, "notes", "save",  vm.saveLabel);
    bind_str(subs, "notes", "delete",vm.deleteLabel);
    bind_str(subs, "notes", "title_placeholder", vm.titlePlaceholder);
    bind_str(subs, "notes", "body_placeholder",  vm.bodyPlaceholder);
    // Note list: push as newline-joined titles.
    auto sync_notes = [&vm]() {
        std::string joined;
        for (const auto& n : vm.notes.snapshot()) {
            if (!joined.empty()) joined += "\n";
            joined += n->title.empty() ? "(untitled)" : n->title;
        }
        push_property("notes", "noteList", joined);
    };
    sync_notes();
    subs.push_back(vm.notes.on_any_change(
        [sync_notes]() { sync_notes(); }));
}

void set_notes_text(NotesVm& vm, const std::string& propName,
                    const std::string& value) {
    if (propName == "editTitle") vm.editTitle.set(value);
    else if (propName == "editBody")  vm.editBody.set(value);
}

void exec_notes_command(NotesVm& vm, const std::string& cmdName) {
    if (cmdName == "addNote")        vm.addNote.execute();
    else if (cmdName == "saveNote")   vm.saveNote.execute();
    else if (cmdName == "deleteSelected") vm.deleteSelected.execute();
}

void register_notes_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_notes(bus, static_cast<NotesVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_notes_text(static_cast<NotesVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_notes_command(static_cast<NotesVm&>(vm), cmdName);
    };
    table.emplace("notes", b);
}

}  // namespace wb::notes
