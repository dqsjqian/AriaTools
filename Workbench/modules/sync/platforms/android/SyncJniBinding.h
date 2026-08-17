// ────────────────────────────────────────────────────────────────────────────
//  SyncJniBinding — Android JNI side-channel binding for the sync module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/SyncVm.h"

#include <string>
#include <vector>

namespace wb::sync {

/// Subscribe the module's reactive state → push_property.
void subscribe_sync(aria::runtime::EventBus& bus, SyncVm& vm,
                    std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_sync_text(SyncVm& vm, const std::string& propName,
                   const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_sync_command(SyncVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "sync" module id.
void register_sync_binding(wb::jni::BindingTable& table);

}  // namespace wb::sync
