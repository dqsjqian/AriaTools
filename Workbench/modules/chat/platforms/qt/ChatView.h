#pragma once
//
// ChatView — Qt view for the "chat" module.
//
// Decomposed into sub-views, each owning its own widget + bindings:
//   PublisherView   — username + draft + send button
//   SubscriberView  — list of received messages
//
// The top-level ChatView assembles them under the shared ChatVm hint
// banner and wires the two child VMs. register_chat_view() is a thin
// entry point that constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>
#include <QListView>

#include <memory>

namespace wb::chat { class ChatVm; class ChatPublisherVm; class ChatSubscriberVm; }

namespace wb::chat::qtview {

class PublisherView {
public:
    PublisherView(ChatPublisherVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return row_; }
private:
    QWidget* row_;
};

class SubscriberView {
public:
    SubscriberView(ChatSubscriberVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return list_; }
private:
    QListView* list_;
};

class ChatView {
public:
    ChatView(ChatVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
    std::unique_ptr<PublisherView>  publisher_;
    std::unique_ptr<SubscriberView> subscriber_;
};

}  // namespace wb::chat::qtview
