#include "SettingsView.h"
#include "support/IosUi.h"
#include "viewmodels/SettingsVm.h"

#include "aria/binding/binding_engine.hpp"

#include <string>

namespace wb::settings::iosview {

SettingsView::SettingsView(SettingsVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*  title   = wb::ios::ui::make_title(@"");
    UILabel*  hint    = wb::ios::ui::make_label(@"");
    UILabel*  langLbl = wb::ios::ui::make_label(@"");
    UIButton* zh      = wb::ios::ui::make_button(@"");
    UIButton* en      = wb::ios::ui::make_button(@"");

    vc_ = wb::ios::ui::make_stack_vc(@[title, hint, langLbl, zh, en]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.languageLabel, wb::ios::ui::view_for(langLbl));
    // Language switch: pass the language code as a binding parameter to the command.
    be.bind_text_oneway(vm.zhLabel, wb::ios::ui::view_for(zh));
    be.bind_text_oneway(vm.enLabel, wb::ios::ui::view_for(en));
    be.bind_command(vm.switchLanguage, wb::ios::ui::view_for(zh), std::string("zh-CN"));
    be.bind_command(vm.switchLanguage, wb::ios::ui::view_for(en), std::string("en"));
}

}  // namespace wb::settings::iosview
