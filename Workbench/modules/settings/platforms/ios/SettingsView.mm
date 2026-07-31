#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SettingsVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::settings::iosview {
static UIViewController* build(SettingsVm& vm, aria::binding::BindingEngine& be) {
    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     hint    = wb::ios::ui::make_label(@"");
    UITextField* dataDir = wb::ios::ui::make_field(@"");
    UITextField* remote  = wb::ios::ui::make_field(@"");
    UITextField* branch  = wb::ios::ui::make_field(@"");
    UITextField* user    = wb::ios::ui::make_field(@"");
    UITextField* token   = wb::ios::ui::make_field(@""); token.secureTextEntry = YES;
    UISwitch*    autoSw  = [[UISwitch alloc] init];
    UIButton*    save    = wb::ios::ui::make_button(@"");
    UILabel*     status  = wb::ios::ui::make_label(@"");
    auto* vc = wb::ios::ui::make_stack_vc(@[title, hint, dataDir, remote, branch,
                                            user, token, autoSw, save, status]);
    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.saveLabel, wb::ios::ui::view_for(save));
    be.bind_text(vm.dataDir, wb::ios::ui::view_for(dataDir));
    be.bind_text(vm.remoteUrl, wb::ios::ui::view_for(remote));
    be.bind_text(vm.branch, wb::ios::ui::view_for(branch));
    be.bind_text(vm.username, wb::ios::ui::view_for(user));
    be.bind_text(vm.token, wb::ios::ui::view_for(token));
    be.bind_bool(vm.autoSync, wb::ios::ui::view_for(autoSw));
    be.bind_command(vm.save, wb::ios::ui::view_for(save));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
    return vc;
}
}  // namespace wb::settings::iosview

namespace wb::settings {
void register_settings_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "settings", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<SettingsVm&>(vm), be);
        });
}
}  // namespace wb::settings
