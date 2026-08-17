#include "DashboardView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/DashboardVm.h"

#include "module_api/NavigationEntryVm.h"

#include "aria/binding/binding_engine.hpp"

// Receives swipe-down (interactive) dismiss of the presented modal so the
// stack entry can be popped — same user intent as pressing Done.
// Must live at global scope: Objective-C declarations are not allowed inside
// C++ namespaces (including anonymous ones).
@interface WbNavDismissDelegate : NSObject <UIAdaptivePresentationControllerDelegate>
@property (nonatomic, copy) void (^onDismiss)(void);
@end

@implementation WbNavDismissDelegate
- (void)presentationControllerDidDismiss:(UIPresentationController *)presentationController {
    (void)presentationController;
    if (self.onDismiss) self.onDismiss();
}
@end

// iOS 27 / Xcode-beta: [[UIViewController alloc] init]'s default loadView
// doesn't materialize the view until the VC is added to a hierarchy, and
// even [vc loadViewIfNeeded] still leaves vc.view = nil. Override loadView
// so the dashboard's root view is reliably allocated on construction.
@interface WbDashboardVC : UIViewController @end
@implementation WbDashboardVC
- (void)loadView { self.view = [[UIView alloc] init]; }
@end

namespace wb::dashboard::iosview {

DashboardView::DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil), pageHost_(nil),
      presentedSentinels_([[NSMutableArray alloc] init]) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* info  = wb::ios::ui::make_label(@"");
    UIButton* openCartBtn   = wb::ios::ui::make_button(@"");
    UIButton* modalCartBtn  = wb::ios::ui::make_button(@"");
    UIButton* windowCartBtn = wb::ios::ui::make_button(@"");
    UIButton* backBtn       = wb::ios::ui::make_button(@"");

    // Custom layout: the make_stack_vc helper is for plain pages where
    // arrangedSubviews stack tightly and the scrollView carries any overflow.
    // For the dashboard we need the embedded page host to FILL the space
    // below the buttons (otherwise pushed pages render into a 0-height
    // area), but a stack with a flexible child + stack.heightAnchor >=
    // scroll.heightAnchor breaks every OTHER page's layout (extra space
    // gets redistributed to arrangedSubviews). So we lay out the dashboard
    // manually: a top stack (title/info/buttons, intrinsic height) and the
    // host as a separate child VC that takes everything below it.
        // vc_ member is __weak (the tab bar / IosShell owns the controller; the
    // View only observes it). alloc/init does NOT go through autorelease, so
    // assigning straight to the weak member would deallocate the controller
    // immediately. Hold it in a local strong variable through construction —
    // IosShell::build_root takes ownership right after.
    WbDashboardVC* vc = [[WbDashboardVC alloc] initWithNibName:nil bundle:nil];
    vc.view = [[UIView alloc] init];
    vc.view.backgroundColor = [UIColor systemBackgroundColor];

    UIStackView* top = [[UIStackView alloc] initWithArrangedSubviews:@[
        title, info, openCartBtn, modalCartBtn, windowCartBtn, backBtn
    ]];
    top.axis = UILayoutConstraintAxisVertical;
    top.spacing = 12;
    top.alignment = UIStackViewAlignmentFill;
    top.translatesAutoresizingMaskIntoConstraints = NO;
    [vc.view addSubview:top];

    UIViewController* host = [[UIViewController alloc] init];
    host.view.backgroundColor = [UIColor systemBackgroundColor];
    host.view.translatesAutoresizingMaskIntoConstraints = NO;
    [vc.view addSubview:host.view];
    [vc addChildViewController:host];
    [host didMoveToParentViewController:vc];
    pageHost_ = host;

    // Use the classic constraintWithItem:attribute: API instead of the anchor
    // factory: iOS 27 / Xcode-beta anchor factories were throwing
    // 'cannot be made to a constant' even with non-nil anchors, breaking
    // the dashboard. Old API takes views directly and works reliably.
    UIView* parent = vc.view;
    [NSLayoutConstraint activateConstraints:@[
        [NSLayoutConstraint constraintWithItem:top
                                    attribute:NSLayoutAttributeTop
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeTop
                                   multiplier:1.0
                                     constant:60],
        [NSLayoutConstraint constraintWithItem:top
                                    attribute:NSLayoutAttributeLeading
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeLeading
                                   multiplier:1.0
                                     constant:16],
        [NSLayoutConstraint constraintWithItem:top
                                    attribute:NSLayoutAttributeTrailing
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeTrailing
                                   multiplier:1.0
                                     constant:-16],

        [NSLayoutConstraint constraintWithItem:host.view
                                    attribute:NSLayoutAttributeTop
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:top
                                    attribute:NSLayoutAttributeBottom
                                   multiplier:1.0
                                     constant:16],
        [NSLayoutConstraint constraintWithItem:host.view
                                    attribute:NSLayoutAttributeLeading
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeLeading
                                   multiplier:1.0
                                     constant:0],
        [NSLayoutConstraint constraintWithItem:host.view
                                    attribute:NSLayoutAttributeTrailing
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeTrailing
                                   multiplier:1.0
                                     constant:0],
        [NSLayoutConstraint constraintWithItem:host.view
                                    attribute:NSLayoutAttributeBottom
                                    relatedBy:NSLayoutRelationEqual
                                       toItem:parent
                                    attribute:NSLayoutAttributeBottom
                                   multiplier:1.0
                                     constant:0],
    ]];
    // Hand the controller to the weak member now that construction is done;
    // IosShell::build_root will take the strong ownership right after.
    vc_ = vc;

    // The View only fires Commands; the VM decides which module to push and
    // HOW to present it. Buttons for the three presentation demos.
    be.bind_text_oneway(vm.welcome, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ios::ui::view_for(info));
    be.bind_text_oneway(vm.openCartLabel,   wb::ios::ui::view_for(openCartBtn));
    be.bind_text_oneway(vm.modalCartLabel,  wb::ios::ui::view_for(modalCartBtn));
    be.bind_text_oneway(vm.windowCartLabel, wb::ios::ui::view_for(windowCartBtn));
    be.bind_text_oneway(vm.navBackLabel,    wb::ios::ui::view_for(backBtn));
    be.bind_command(vm.openCart,   wb::ios::ui::view_for(openCartBtn));
    be.bind_command(vm.modalCart,  wb::ios::ui::view_for(modalCartBtn));
    be.bind_command(vm.windowCart, wb::ios::ui::view_for(windowCartBtn));
    be.bind_command(vm.navBack,    wb::ios::ui::view_for(backBtn));

    // Render the navigator's current entry by its Presentation kind.
    auto& navCurrent = vm.navigator().current();
    auto render_current = [this, &vm, &be](const std::shared_ptr<aria::binding::ViewModel>& current) {
        // If the nav changed underneath a presented modal (e.g. the user hit
        // the home Back button), tear the modal down first. Code-driven
        // dismissal does NOT fire presentationControllerDidDismiss, so this
        // cannot double-pop.
        if (vc_.presentedViewController) {
            [vc_ dismissViewControllerAnimated:NO completion:nil];
        }

        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(current.get());
        UIViewController* page = nil;
        if (entry) {
            page = wb::ios::UIViewFactory::instance().build(
                entry->module_id(), entry->inner(), be);
        }

        // Clear the embedded page container (Push kind) — at root it shows
        // nothing. Embedded children live under pageHost_ (the container in
        // the home layout), NOT over the whole dashboard — otherwise the
        // home buttons get covered and the user cannot hit Back.
        for (UIViewController* child in pageHost_.childViewControllers) {
            [child willMoveToParentViewController:nil];
            [child.view removeFromSuperview];
            [child removeFromParentViewController];
        }

        if (!entry || !page) return;

        switch (entry->presentation()) {
            case wb::module_api::Presentation::Push: {
                // Embed inside pageHost_, keeping the home title/buttons and
                // the tab bar visible above/below.
                [pageHost_ addChildViewController:page];
                page.view.frame = pageHost_.view.bounds;
                page.view.autoresizingMask =
                    UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
                [pageHost_.view addSubview:page.view];
                [page didMoveToParentViewController:pageHost_];
                break;
            }
            case wb::module_api::Presentation::Modal:
            case wb::module_api::Presentation::Window: {
                // Mobile has no separate-window concept: Window falls back to
                // a full modal presentation, identical handling.
                UINavigationController* navC =
                    [[UINavigationController alloc] initWithRootViewController:page];
                navC.modalPresentationStyle = UIModalPresentationAutomatic;

                __weak UIViewController* weakVC = vc_;
                page.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc]
                    initWithBarButtonSystemItem:UIBarButtonSystemItemDone
                    primaryAction:[UIAction actionWithHandler:^(UIAction* _Nonnull action) {
                        (void)action;
                        [weakVC dismissViewControllerAnimated:YES completion:nil];
                        vm.navBack.execute();
                    }]];

                // Swipe-down interactive dismiss -> pop the stack entry too.
                WbNavDismissDelegate* d = [WbNavDismissDelegate new];
                d.onDismiss = ^{
                    vm.navBack.execute();
                };
                navC.presentationController.delegate = d;
                [presentedSentinels_ addObject:d];

                [vc_ presentViewController:navC animated:YES completion:nil];
                break;
            }
        }
    };
    subscriptions_.push_back(navCurrent.on_changed(
        [render_current](const std::shared_ptr<aria::binding::ViewModel>& c) {
            render_current(c);
        }));
    render_current(navCurrent.get());
}

}  // namespace wb::dashboard::iosview
