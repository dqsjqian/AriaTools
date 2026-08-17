// ────────────────────────────────────────────────────────────────────────────
//  SearchJniBinding — Android JNI side-channel binding for the search module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/SearchVm.h"

#include <string>
#include <vector>

namespace wb::search {

/// Subscribe the module's reactive state → push_property.
void subscribe_search(aria::runtime::EventBus& bus, SearchVm& vm,
                      std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_search_text(SearchVm& vm, const std::string& propName,
                     const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_search_command(SearchVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "search" module id.
void register_search_binding(wb::jni::BindingTable& table);

}  // namespace wb::search
