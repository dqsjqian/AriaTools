// ────────────────────────────────────────────────────────────────────────────
//  TipCalcJniBinding — Android JNI side-channel binding for the tipcalc
//  module. See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/TipCalcVm.h"

#include <string>
#include <vector>

namespace wb::tipcalc {

/// Subscribe the module's reactive state → push_property.
void subscribe_tipcalc(aria::runtime::EventBus& bus, TipCalcVm& vm,
                       std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_tipcalc_text(TipCalcVm& vm, const std::string& propName,
                      const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_tipcalc_command(TipCalcVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "tipcalc" module id.
void register_tipcalc_binding(wb::jni::BindingTable& table);

}  // namespace wb::tipcalc
