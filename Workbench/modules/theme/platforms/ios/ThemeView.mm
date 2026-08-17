#include "ThemeView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/ThemeVm.h"
#include "viewmodels/ThemeVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::theme::iosview {

ThemeView::ThemeView(ThemeVmHostVm& host, aria::binding::BindingEngine& be)
    : vc_(nil) {
    auto& vm = host.inner();

    UILabel*  title   = wb::ios::ui::make_title(@"");
    UILabel*  desc    = wb::ios::ui::make_label(@"");
    UILabel*  current = wb::ios::ui::make_label(@"");
    UIButton* pickLight = wb::ios::ui::make_button(@"");
    UIButton* pickDark  = wb::ios::ui::make_button(@"");
    UIButton* pickSolar = wb::ios::ui::make_button(@"");
    UILabel*  cardTitle = wb::ios::ui::make_label(@"");
    UILabel*  cardBody  = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, desc, current, pickLight, pickDark, pickSolar, cardTitle, cardBody]);

    be.bind_text_oneway(host.title,            wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,             wb::ios::ui::view_for(desc));
    be.bind_text_oneway(vm.currentDisplayName, wb::ios::ui::view_for(current));

    // Theme picker buttons — localized labels + command wiring.
    [pickLight setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("theme","theme_light").c_str()]
              forState:UIControlStateNormal];
    [pickDark  setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("theme","theme_dark").c_str()]
              forState:UIControlStateNormal];
    [pickSolar setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("theme","theme_solarized").c_str()]
              forState:UIControlStateNormal];
    be.bind_command(vm.pickLight,     wb::ios::ui::view_for(pickLight));
    be.bind_command(vm.pickDark,      wb::ios::ui::view_for(pickDark));
    be.bind_command(vm.pickSolarized, wb::ios::ui::view_for(pickSolar));

    // Card text (i18n) — static per language; re-resolved on VC rebuild.
    cardTitle.text = [NSString stringWithUTF8String:wb::i18n::str_in("theme","card_title").c_str()];
    cardBody.text  = [NSString stringWithUTF8String:wb::i18n::str_in("theme","card_body").c_str()];
}

}  // namespace wb::theme::iosview

namespace wb::theme {
void register_theme_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "theme", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<ThemeVmHostVm&>(vm);
            auto* view = new iosview::ThemeView(host, be);
            return view->viewController();
        });
}
}  // namespace wb::theme
