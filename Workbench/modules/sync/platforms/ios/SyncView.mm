#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SyncVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::sync::iosview {
static UIViewController* build(SyncVm& vm, aria::binding::BindingEngine& be) {
    UILabel*  title = wb::ios::ui::make_title(@"");
    UILabel*  hint  = wb::ios::ui::make_label(@"");
    UIButton* sync  = wb::ios::ui::make_button(@"");
    UIButton* pull  = wb::ios::ui::make_button(@"");
    UIButton* push  = wb::ios::ui::make_button(@"");
    UILabel*  status = wb::ios::ui::make_label(@"");
    UILabel*  log   = wb::ios::ui::make_label(@"");
    auto* vc = wb::ios::ui::make_stack_vc(@[title, hint, sync, pull, push, status, log]);
    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.syncLabel, wb::ios::ui::view_for(sync));
    be.bind_text_oneway(vm.pullLabel, wb::ios::ui::view_for(pull));
    be.bind_text_oneway(vm.pushLabel, wb::ios::ui::view_for(push));
    be.bind_command(vm.syncNow, wb::ios::ui::view_for(sync));
    be.bind_command(vm.pullOnly, wb::ios::ui::view_for(pull));
    be.bind_command(vm.pushOnly, wb::ios::ui::view_for(push));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
    be.bind_text_oneway(vm.log, wb::ios::ui::view_for(log));
    return vc;
}
}  // namespace wb::sync::iosview

namespace wb::sync {
void register_sync_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "sync", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<SyncVm&>(vm), be);
        });
}
}  // namespace wb::sync
