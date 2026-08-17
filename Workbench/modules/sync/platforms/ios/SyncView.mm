#include "SyncView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SyncVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::sync::iosview {

SyncView::SyncView(SyncVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     hint    = wb::ios::ui::make_label(@"");
    UITextField* dataDir = wb::ios::ui::make_field(@"");
    UITextField* remote  = wb::ios::ui::make_field(@"");
    UITextField* branch  = wb::ios::ui::make_field(@"");
    UITextField* user    = wb::ios::ui::make_field(@"");
    UITextField* token   = wb::ios::ui::make_field(@""); token.secureTextEntry = YES;
    UISwitch*    autoSw  = [[UISwitch alloc] init];
    UIButton*    saveCfg = wb::ios::ui::make_button(@"");
    UIButton*    sync    = wb::ios::ui::make_button(@"");
    UIButton*    pull    = wb::ios::ui::make_button(@"");
    UIButton*    push    = wb::ios::ui::make_button(@"");
    UILabel*     status  = wb::ios::ui::make_label(@"");
    UILabel*     log     = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(@[title, hint, dataDir, remote, branch,
                                       user, token, autoSw, saveCfg,
                                       sync, pull, push, status, log]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text(vm.dataDir, wb::ios::ui::view_for(dataDir));
    be.bind_text(vm.remoteUrl, wb::ios::ui::view_for(remote));
    be.bind_text(vm.branch, wb::ios::ui::view_for(branch));
    be.bind_text(vm.username, wb::ios::ui::view_for(user));
    be.bind_text(vm.token, wb::ios::ui::view_for(token));
    be.bind_bool(vm.autoSync, wb::ios::ui::view_for(autoSw));
    be.bind_text_oneway(vm.saveLabel, wb::ios::ui::view_for(saveCfg));
    be.bind_command(vm.saveConfig, wb::ios::ui::view_for(saveCfg));

    be.bind_text_oneway(vm.syncLabel, wb::ios::ui::view_for(sync));
    be.bind_text_oneway(vm.pullLabel, wb::ios::ui::view_for(pull));
    be.bind_text_oneway(vm.pushLabel, wb::ios::ui::view_for(push));
    be.bind_command(vm.syncNow, wb::ios::ui::view_for(sync));
    be.bind_command(vm.pullOnly, wb::ios::ui::view_for(pull));
    be.bind_command(vm.pushOnly, wb::ios::ui::view_for(push));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
    be.bind_text_oneway(vm.log, wb::ios::ui::view_for(log));
}

}  // namespace wb::sync::iosview

namespace wb::sync {
void register_sync_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "sync", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::SyncView(static_cast<SyncVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::sync
