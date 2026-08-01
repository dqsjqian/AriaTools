#pragma once
//
// IosUi — Shared helpers for the iOS View layer: UIKitView lifetime management + common control construction + vertical stack container.
//
#import <UIKit/UIKit.h>

#include "aria/adapters/uikit/UIKitAdapter.hpp"

namespace wb::ios::ui {

/// Wrap a UIView into an aria UIKitView (lifetime managed, avoids dangling).
aria::adapters::uikit::UIKitView& view_for(UIView* v);

UILabel*     make_label(NSString* text);
UILabel*     make_title(NSString* text);
UIButton*    make_button(NSString* title);
UITextField* make_field(NSString* placeholder);

/// Host child views in a vertical UIStackView + UIScrollView; returns a VC.
UIViewController* make_stack_vc(NSArray<UIView*>* children);

}  // namespace wb::ios::ui
