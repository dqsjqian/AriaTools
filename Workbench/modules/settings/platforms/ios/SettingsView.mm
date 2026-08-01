#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SettingsVm.h"

#include "aria/binding/binding_engine.hpp"

#include <string>

namespace wb::settings::iosview {
static UIViewController* build(SettingsVm& vm, aria::binding::BindingEngine& be) {
    UILabel*  title   = wb::ios::ui::make_title(@"");
    UILabel*  hint    = wb::ios::ui::make_label(@"");
    UILabel*  langLbl = wb::ios::ui::make_label(@"");
    UIButton* zh      = wb::ios::ui::make_button(@"简体中文");
    UIButton* en      = wb::ios::ui::make_button(@"English");

    auto* vc = wb::ios::ui::make_stack_vc(@[title, hint, langLbl, zh, en]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.languageLabel, wb::ios::ui::view_for(langLbl));
    // 语言切换：把语言码作为绑定参数传给命令。
    be.bind_command(vm.switchLanguage, wb::ios::ui::view_for(zh), std::string("zh-CN"));
    be.bind_command(vm.switchLanguage, wb::ios::ui::view_for(en), std::string("en"));
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
