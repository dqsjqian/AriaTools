// ────────────────────────────────────────────────────────────────────────────
//  NotesJniBinding — Android JNI side-channel binding for the notes module.
//
//  subscribe_notes() wires NotesVm reactive properties to the string wire
//  format (JniBridge.onPropertyChanged); set_notes_text() / exec_notes_command()
//  route Kotlin writes and commands to the underlying Property/Command.
//
//  See platform/android/jni/JniBind.h for the shared helpers and the registry
//  contract. jni_bridge.cpp and AndroidShell.cpp dispatch to this module by
//  module id through register_notes_binding().
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/NotesVm.h"

#include <string>
#include <vector>

namespace wb::notes {

/// Subscribe the module's reactive state → push_property.
void subscribe_notes(aria::runtime::EventBus& bus, NotesVm& vm,
                     std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_notes_text(NotesVm& vm, const std::string& propName,
                    const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_notes_command(NotesVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "notes" module id.
void register_notes_binding(wb::jni::BindingTable& table);

}  // namespace wb::notes
