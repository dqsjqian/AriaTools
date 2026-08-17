#pragma once
//
// ChatView — Qt view for the "chat" module.
//
// Decomposed into sub-views, each owning its own widget + bindings:
//   publisher view   — username + draft + send button
//   subscriber view  — list of received messages
//
// The top-level ChatView constructor assembles them under the shared ChatVm
// hint banner and wires the two child VMs.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::chat { class ChatVm; }

namespace wb::chat::qtview {

class ChatView {
public:
    ChatView(ChatVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::chat::qtview
