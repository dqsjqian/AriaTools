// ────────────────────────────────────────────────────────────────────────────
//  ChatJniBinding — Android JNI side-channel binding for the chat module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/ChatVm.h"

#include <string>
#include <vector>

namespace wb::chat {

/// Subscribe the module's reactive state → push_property.
void subscribe_chat(aria::runtime::EventBus& bus, ChatVm& vm,
                    std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_chat_text(ChatVm& vm, const std::string& propName,
                   const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_chat_command(ChatVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "chat" module id.
void register_chat_binding(wb::jni::BindingTable& table);

}  // namespace wb::chat
