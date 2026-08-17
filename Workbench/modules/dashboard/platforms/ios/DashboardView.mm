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

    UIViewController* host = [[UIViewController alloc] init];
    host.view.backgroundColor = [UIColor systemBackgroundColor];
    pageHost_ = host;

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, info, openCartBtn, modalCartBtn, windowCartBtn, backBtn, host.view]);

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
