// ────────────────────────────────────────────────────────────────────────────
//  ToolsJniBinding — Android JNI side-channel binding for the tools module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/ToolsVm.h"

#include <string>
#include <vector>

namespace wb::tools {

/// Subscribe the module's reactive state → push_property.
void subscribe_tools(aria::runtime::EventBus& bus, ToolsVm& vm,
                     std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_tools_text(ToolsVm& vm, const std::string& propName,
                    const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_tools_command(ToolsVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "tools" module id.
void register_tools_binding(wb::jni::BindingTable& table);

}  // namespace wb::tools
