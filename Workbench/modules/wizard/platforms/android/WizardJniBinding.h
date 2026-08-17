// ────────────────────────────────────────────────────────────────────────────
//  WizardJniBinding — Android JNI side-channel binding for the wizard module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/WizardVmHostVm.h"

#include <string>
#include <vector>

namespace wb::wizard {

/// Subscribe the module's reactive state → push_property.
void subscribe_wizard(aria::runtime::EventBus& bus, WizardVmHostVm& vm,
                      std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_wizard_text(WizardVmHostVm& vm, const std::string& propName,
                     const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_wizard_command(WizardVmHostVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "wizard" module id.
void register_wizard_binding(wb::jni::BindingTable& table);

}  // namespace wb::wizard
