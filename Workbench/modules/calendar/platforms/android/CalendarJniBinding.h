// ────────────────────────────────────────────────────────────────────────────
//  CalendarJniBinding — Android JNI side-channel binding for the calendar
//  module. See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/CalendarVm.h"

#include <string>
#include <vector>

namespace wb::calendar {

/// Subscribe the module's reactive state → push_property.
void subscribe_calendar(aria::runtime::EventBus& bus, CalendarVm& vm,
                        std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_calendar_text(CalendarVm& vm, const std::string& propName,
                       const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_calendar_command(CalendarVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "calendar" module id.
void register_calendar_binding(wb::jni::BindingTable& table);

}  // namespace wb::calendar
