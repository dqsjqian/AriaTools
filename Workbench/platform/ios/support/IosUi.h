#pragma once
//
// IosUi — Shared helpers for the iOS View layer: UIKitView lifetime management + common control construction + vertical stack container.
//
#import <UIKit/UIKit.h>

#include "aria/adapters/uikit/UIKitAdapter.hpp"
#include "aria/subscription.hpp"

#include <vector>

namespace wb::ios::ui {

/// Wrap a UIView into an aria UIKitView (lifetime managed, avoids dangling).
aria::adapters::uikit::UIKitView& view_for(UIView* v);

UILabel*     make_label(NSString* text);
UILabel*     make_title(NSString* text);
UIButton*    make_button(NSString* title);
UITextField* make_field(NSString* placeholder);

/// Host child views in a vertical UIStackView + UIScrollView; returns a VC.
UIViewController* make_stack_vc(NSArray<UIView*>* children);

/// Process-wide keepalive bag for subscriptions that have no natural owner
/// (e.g. Computed::on_changed in iOS views, where there is no per-VC
/// SubscriptionBag like Qt's subs_attached_to). Subscriptions pushed here
/// live until the process exits; acceptable for a demo app where module
/// VMs outlive views anyway.
std::vector<aria::Subscription>& subs_keepalive();

}  // namespace wb::ios::ui
