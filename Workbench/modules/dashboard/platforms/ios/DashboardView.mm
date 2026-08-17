#include "DashboardView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/DashboardVm.h"

#include "module_api/NavigationEntryVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::dashboard::iosview {

DashboardView::DashboardView(DashboardVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil), pageHost_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* info  = wb::ios::ui::make_label(@"");
    UIButton* openCartBtn = wb::ios::ui::make_button(@"");
    UIButton* backBtn     = wb::ios::ui::make_button(@"");

    UIViewController* host = [[UIViewController alloc] init];
    host.view.backgroundColor = [UIColor systemBackgroundColor];
    pageHost_ = host;

    vc_ = wb::ios::ui::make_stack_vc(@[title, info, openCartBtn, backBtn, host.view]);

    // The View only fires Commands; the VM decides which module to push.
    be.bind_text_oneway(vm.welcome, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ios::ui::view_for(info));
    be.bind_text_oneway(vm.openCartLabel, wb::ios::ui::view_for(openCartBtn));
    be.bind_text_oneway(vm.navBackLabel,  wb::ios::ui::view_for(backBtn));
    be.bind_command(vm.openCart, wb::ios::ui::view_for(openCartBtn));
    be.bind_command(vm.navBack,  wb::ios::ui::view_for(backBtn));

    // Render the navigator's current entry by embedding the target module's
    // VC as a child.
    auto& navCurrent = vm.navigator().current();
    auto render_current = [this, &vm, &be](const std::shared_ptr<aria::binding::ViewModel>& current) {
        UIViewController* page = nil;
        auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(current.get());
        if (entry) {
            page = wb::ios::UIViewFactory::instance().build(
                entry->module_id(), entry->inner(), be);
        }
        for (UIViewController* child in vc_.childViewControllers) {
            [child willMoveToParentViewController:nil];
            [child.view removeFromSuperview];
            [child removeFromParentViewController];
        }
        if (page) {
            [vc_ addChildViewController:page];
            page.view.frame = vc_.view.bounds;
            [vc_.view addSubview:page.view];
            [page didMoveToParentViewController:vc_];
        }
    };
    subscriptions_.push_back(navCurrent.on_changed(
        [render_current](const std::shared_ptr<aria::binding::ViewModel>& c) {
            render_current(c);
        }));
    render_current(navCurrent.get());
}

}  // namespace wb::dashboard::iosview
