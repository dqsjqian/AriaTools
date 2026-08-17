// ────────────────────────────────────────────────────────────────────────────
//  SignupJniBinding — Android JNI side-channel binding for the signup module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/SignupVm.h"

#include <string>
#include <vector>

namespace wb::signup {

/// Subscribe the module's reactive state → push_property.
void subscribe_signup(aria::runtime::EventBus& bus, SignupVm& vm,
                      std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_signup_text(SignupVm& vm, const std::string& propName,
                     const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_signup_command(SignupVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "signup" module id.
void register_signup_binding(wb::jni::BindingTable& table);

}  // namespace wb::signup
