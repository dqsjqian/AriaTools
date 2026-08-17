// ────────────────────────────────────────────────────────────────────────────
//  CartJniBinding — Android JNI side-channel binding for the cart module.
//  See NotesJniBinding.h for the shared pattern and JniBind.h for the
//  registry contract.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include "jni/JniBind.h"
#include "viewmodels/CartVm.h"

#include <string>
#include <vector>

namespace wb::cart {

/// Subscribe the module's reactive state → push_property.
void subscribe_cart(aria::runtime::EventBus& bus, CartVm& vm,
                    std::vector<aria::Subscription>& subs);

/// Kotlin→C++: set a user-editable string property.
void set_cart_text(CartVm& vm, const std::string& propName,
                   const std::string& value);

/// Kotlin→C++: execute a parameterless command.
void exec_cart_command(CartVm& vm, const std::string& cmdName);

/// Fill the registry entry for the "cart" module id.
void register_cart_binding(wb::jni::BindingTable& table);

}  // namespace wb::cart
