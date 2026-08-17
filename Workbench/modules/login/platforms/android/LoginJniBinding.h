// ────────────────────────────────────────────────────────────────────────────
//  LoginJniBinding — Android JNI side-channel binding for the login module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/LoginVm.h"

#include <string>
#include <vector>

namespace wb::login {

/// Subscribe the module's reactive state → push_property.
void subscribe_login(aria::runtime::EventBus& bus, LoginVm& vm,
                     std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_login_text(LoginVm& vm, const std::string& propName,
                    const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_login_command(LoginVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "login" module id.
void register_login_binding(wb::jni::BindingTable& table);

}  // namespace wb::login
