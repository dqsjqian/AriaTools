#include "support/IosUi.h"

#include <memory>
#include <vector>

namespace wb::ios::ui {

using aria::adapters::uikit::UIKitView;

namespace {
std::vector<std::shared_ptr<UIKitView>>& keepalive() {
    static std::vector<std::shared_ptr<UIKitView>> v;
    return v;
}
}  // namespace

UIKitView& view_for(UIView* v) {
    auto p = std::make_shared<UIKitView>(v);
    auto& ref = *p;
    keepalive().push_back(std::move(p));
    return ref;
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

std::vector<aria::Subscription>& subs_keepalive() {
    static std::vector<aria::Subscription> v;
    return v;
}

}  // namespace wb::ios::ui
