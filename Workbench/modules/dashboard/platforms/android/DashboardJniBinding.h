// ────────────────────────────────────────────────────────────────────────────
//  DashboardJniBinding — Android JNI side-channel binding for the dashboard
//  module. Includes the cross-module navigation mirrors (navCurrentModule /
//  navDepth) and the openCart / navBack command routing.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/DashboardVm.h"

#include <string>
#include <vector>

namespace wb::dashboard {

/// Subscribe the module's reactive state → push_property.
void subscribe_dashboard(aria::runtime::EventBus& bus, DashboardVm& vm,
                         std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_dashboard_text(DashboardVm& vm, const std::string& propName,
                        const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_dashboard_command(DashboardVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "dashboard" module id.
void register_dashboard_binding(wb::jni::BindingTable& table);

}  // namespace wb::dashboard
