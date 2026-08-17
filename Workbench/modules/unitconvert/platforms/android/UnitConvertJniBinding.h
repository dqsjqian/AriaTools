// ────────────────────────────────────────────────────────────────────────────
//  UnitConvertJniBinding — Android JNI side-channel binding for the
//  unitconvert module. See NotesJniBinding.h for the shared pattern and
//  JniBind.h for the registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/UnitConvertVm.h"

#include <string>
#include <vector>

namespace wb::unitconvert {

/// Subscribe the module's reactive state → push_property.
void subscribe_unitconvert(aria::runtime::EventBus& bus, UnitConvertVm& vm,
                           std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_unitconvert_text(UnitConvertVm& vm, const std::string& propName,
                          const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_unitconvert_command(UnitConvertVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "unitconvert" module id.
void register_unitconvert_binding(wb::jni::BindingTable& table);

}  // namespace wb::unitconvert
