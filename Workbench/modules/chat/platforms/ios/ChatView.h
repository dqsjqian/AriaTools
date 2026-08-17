#pragma once
//
// ChatView — iOS UIKit view for the "chat" module.
//
// Decomposed into sub-views (mirroring the Qt ChatView structure):
//   PublisherView   — user/draft fields + send button (ChatPublisherVm)
//   SubscriberView  — placeholder panel for the subscriber side
//                     (ChatSubscriberVm). iOS has no list adapter yet,
//                     so this only shows a section header.
//
// The top-level ChatView assembles them into a vertical stack VC and
// wires the shared parent ChatVm (title/desc).
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::chat { class ChatVm; }

namespace wb::chat::iosview {

// ─── Sub-views (each builds a UIView*) ────────────────────────────────────

class PublisherView {
public:
    PublisherView(ChatVm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return view_; }
private:
    UIView* view_;
};

class SubscriberView {
public:
    SubscriberView(ChatVm& vm, aria::binding::BindingEngine& be);
    UIView* view() const { return view_; }
private:
    UIView* view_;
};

// ─── Top-level ChatView ───────────────────────────────────────────────────

class ChatView {
public:
    ChatView(ChatVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::chat::iosview
