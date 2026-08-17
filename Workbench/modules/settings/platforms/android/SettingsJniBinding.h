// ────────────────────────────────────────────────────────────────────────────
//  SettingsJniBinding — Android JNI side-channel binding for the settings
//  module. See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/SettingsVm.h"

#include <string>
#include <vector>

namespace wb::settings {

/// Subscribe the module's reactive state → push_property.
void subscribe_settings(aria::runtime::EventBus& bus, SettingsVm& vm,
                        std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_settings_text(SettingsVm& vm, const std::string& propName,
                       const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_settings_command(SettingsVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "settings" module id.
void register_settings_binding(wb::jni::BindingTable& table);

}  // namespace wb::settings
