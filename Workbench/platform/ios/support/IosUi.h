#pragma once
//
// IosUi — iOS View 层共享工具：UIKitView 生命周期托管 + 常用控件构造 + 垂直栈容器。
//
#import <UIKit/UIKit.h>

#include "aria/adapters/uikit/UIKitAdapter.hpp"

namespace wb::ios::ui {

/// 把 UIView 包成 aria UIKitView（生命周期托管，避免悬垂）。
aria::adapters::uikit::UIKitView& view_for(UIView* v);

UILabel*     make_label(NSString* text);
UILabel*     make_title(NSString* text);
UIButton*    make_button(NSString* title);
UITextField* make_field(NSString* placeholder);

/// 用垂直 UIStackView + UIScrollView 承载子视图，返回一个 VC。
UIViewController* make_stack_vc(NSArray<UIView*>* children);

}  // namespace wb::ios::ui
