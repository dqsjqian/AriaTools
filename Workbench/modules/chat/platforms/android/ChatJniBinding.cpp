// ────────────────────────────────────────────────────────────────────────────
//  ChatJniBinding.cpp — see ChatJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/ChatJniBinding.h"

namespace wb::chat {

void subscribe_chat(aria::runtime::EventBus& bus, ChatVm& vm,
                    std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::push_property;

    bind_str(subs, "chat", "title", vm.title);
    bind_str(subs, "chat", "desc",  vm.desc);
    bind_str(subs, "chat", "user",  vm.publisher->user);
    bind_str(subs, "chat", "draft", vm.publisher->draft);
    // Message list: push as a newline-joined string so the
    // Compose LazyColumn can render each line. Resync on any
    // list mutation (Insert/Remove/ItemChanged).
    auto sync_messages = [&vm]() {
        std::string joined;
        for (const auto& m : vm.subscriber->messages.snapshot()) {
            if (!joined.empty()) joined += "\n";
            joined += m->user + ": " + m->text;
        }
        push_property("chat", "messages", joined);
    };
    sync_messages();
    subs.push_back(vm.subscriber->messages.on_any_change(
        [sync_messages]() { sync_messages(); }));
}

void set_chat_text(ChatVm& vm, const std::string& propName,
                   const std::string& value) {
    if (propName == "user")      vm.publisher->user.set(value);
    else if (propName == "draft") vm.publisher->draft.set(value);
}

void exec_chat_command(ChatVm& vm, const std::string& cmdName) {
    if (cmdName == "send") vm.publisher->send.execute();
}

void register_chat_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_chat(bus, static_cast<ChatVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_chat_text(static_cast<ChatVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_chat_command(static_cast<ChatVm&>(vm), cmdName);
    };
    table.emplace("chat", b);
}

}  // namespace wb::chat
