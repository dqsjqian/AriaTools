#pragma once
//
// ChatView — Qt view for the "chat" module (Aria free-function view).
//
// Decomposed into sub-views, each owning its own widget + bindings:
//   publisher view   — username + draft + send button
//   subscriber view  — list of received messages
//
// The top-level build_view() assembles them under the shared ChatVm hint
// banner and wires the two child VMs. register_chat_view() registers the
// builder with the QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::chat { class ChatVm; }

namespace wb::chat::qtview {

QWidget* build_view(ChatVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::chat::qtview
