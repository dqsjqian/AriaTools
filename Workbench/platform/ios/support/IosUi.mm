#include "support/IosUi.h"

#import <objc/runtime.h>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>

@interface WbIosCppOwnerLifetime : NSObject
- (instancetype)initWithOwner:(std::shared_ptr<void>)owner;
@end

@implementation WbIosCppOwnerLifetime {
    std::shared_ptr<void> owner_;
}
- (instancetype)initWithOwner:(std::shared_ptr<void>)owner {
    if ((self = [super init])) owner_ = std::move(owner);
    return self;
}
@end

@interface WbIosUIKitViewStore : NSObject
- (aria::adapters::uikit::UIKitView&)viewFor:(UIView*)view;
@end

@implementation WbIosUIKitViewStore {
    std::unordered_map<std::uintptr_t,
        std::shared_ptr<aria::adapters::uikit::UIKitView>> views_;
}
- (aria::adapters::uikit::UIKitView&)viewFor:(UIView*)view {
    const auto key = reinterpret_cast<std::uintptr_t>((__bridge void*)view);
    auto [it, inserted] = views_.try_emplace(key);
    if (inserted) {
        it->second = std::make_shared<aria::adapters::uikit::UIKitView>(view);
    }
    return *it->second;
}
@end

namespace wb::ios::ui {

using aria::adapters::uikit::UIKitView;

UIKitView& view_for(UIView* v) {
    static char store_key;
    UIView* root = v;
    while (root.superview) root = root.superview;

    auto* store = static_cast<WbIosUIKitViewStore*>(
        objc_getAssociatedObject(root, &store_key));
    if (!store) {
        store = [[WbIosUIKitViewStore alloc] init];
        objc_setAssociatedObject(root, &store_key, store,
                                 OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return [store viewFor:v];
}

UILabel* make_label(NSString* text) {
    UILabel* l = [[UILabel alloc] init];
    l.text = text; l.numberOfLines = 0; l.font = [UIFont systemFontOfSize:14];
    return l;
}
UILabel* make_title(NSString* text) {
    UILabel* l = [[UILabel alloc] init];
    l.text = text; l.font = [UIFont boldSystemFontOfSize:22];
    return l;
}
UIButton* make_button(NSString* title) {
    UIButton* b = [UIButton buttonWithType:UIButtonTypeSystem];
    [b setTitle:title forState:UIControlStateNormal];
    return b;
}
UITextField* make_field(NSString* placeholder) {
    UITextField* f = [[UITextField alloc] init];
    f.placeholder = placeholder; f.borderStyle = UITextBorderStyleRoundedRect;
    return f;
}

UIViewController* make_stack_vc(NSArray<UIView*>* children) {
    UIViewController* vc = [[UIViewController alloc] init];
    vc.view.backgroundColor = [UIColor systemBackgroundColor];

    UIStackView* stack = [[UIStackView alloc] init];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.spacing = 12;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    stack.alignment = UIStackViewAlignmentFill;
    for (UIView* c in children) [stack addArrangedSubview:c];

    UIScrollView* scroll = [[UIScrollView alloc] init];
    scroll.translatesAutoresizingMaskIntoConstraints = NO;
    [scroll addSubview:stack];
    [vc.view addSubview:scroll];

    UILayoutGuide* g = vc.view.safeAreaLayoutGuide;
    [NSLayoutConstraint activateConstraints:@[
        [scroll.topAnchor constraintEqualToAnchor:g.topAnchor],
        [scroll.leadingAnchor constraintEqualToAnchor:g.leadingAnchor constant:16],
        [scroll.trailingAnchor constraintEqualToAnchor:g.trailingAnchor constant:-16],
        [scroll.bottomAnchor constraintEqualToAnchor:g.bottomAnchor],
        [stack.topAnchor constraintEqualToAnchor:scroll.topAnchor constant:12],
        [stack.leadingAnchor constraintEqualToAnchor:scroll.leadingAnchor],
        [stack.trailingAnchor constraintEqualToAnchor:scroll.trailingAnchor],
        [stack.bottomAnchor constraintEqualToAnchor:scroll.bottomAnchor],
        [stack.widthAnchor constraintEqualToAnchor:scroll.widthAnchor],
    ]];
    return vc;
}

void attach_owner_erased(UIViewController* vc, std::shared_ptr<void> owner) {
    static char owner_key;
    objc_setAssociatedObject(
        vc,
        &owner_key,
        [[WbIosCppOwnerLifetime alloc] initWithOwner:std::move(owner)],
        OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

}  // namespace wb::ios::ui
