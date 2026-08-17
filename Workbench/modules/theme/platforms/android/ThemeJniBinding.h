// ────────────────────────────────────────────────────────────────────────────
//  ThemeJniBinding — Android JNI side-channel binding for the theme module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/ThemeVm.h"

#include <string>
#include <vector>

namespace wb::theme {

/// Subscribe the module's reactive state → push_property.
void subscribe_theme(aria::runtime::EventBus& bus, ThemeVm& vm,
                     std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_theme_text(ThemeVm& vm, const std::string& propName,
                    const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_theme_command(ThemeVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "theme" module id.
void register_theme_binding(wb::jni::BindingTable& table);

}  // namespace wb::theme
