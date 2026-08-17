#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/WizardVm.h"
#include "viewmodels/WizardVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::wizard::iosview {

static UIViewController* build(WizardVm& vm, WizardVmHostVm& host,
                                aria::binding::BindingEngine& be) {
    UILabel*     title  = wb::ios::ui::make_title(@"");
    UILabel*     desc   = wb::ios::ui::make_label(@"");
    UILabel*     step1Lbl = wb::ios::ui::make_label(@"");
    UITextField* userField = wb::ios::ui::make_field(@"");
    UITextField* mailField = wb::ios::ui::make_field(@"");
    UILabel*     step2Lbl = wb::ios::ui::make_label(@"");
    UIButton*    pickLight = wb::ios::ui::make_button(@"");
    UIButton*    pickDark  = wb::ios::ui::make_button(@"");
    UIButton*    pickSolar = wb::ios::ui::make_button(@"");
    UILabel*     step3Lbl = wb::ios::ui::make_label(@"");
    UIButton*    finishBtn = wb::ios::ui::make_button(@"");
    UILabel*     resultLbl = wb::ios::ui::make_label(@"");

    auto* vc = wb::ios::ui::make_stack_vc(
        @[title, desc,
          step1Lbl, userField, mailField,
          step2Lbl, pickLight, pickDark, pickSolar,
          step3Lbl, finishBtn, resultLbl]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));

    // Step labels (i18n).
    step1Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step1").c_str()];
    step2Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step2").c_str()];
    step3Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step3").c_str()];

    // Step 1: username + email.
    userField.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","username").c_str()];
    mailField.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","email").c_str()];
    be.bind_text(vm.draft->username, wb::ios::ui::view_for(userField));
    be.bind_text(vm.draft->email,    wb::ios::ui::view_for(mailField));

    // Step 2: theme picker — three buttons drive Step2Vm commands.
    [pickLight setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_light").c_str()]
              forState:UIControlStateNormal];
    [pickDark  setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_dark").c_str()]
              forState:UIControlStateNormal];
    [pickSolar setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_solarized").c_str()]
              forState:UIControlStateNormal];
    be.bind_command(vm.step2->pickLight,     wb::ios::ui::view_for(pickLight));
    be.bind_command(vm.step2->pickDark,      wb::ios::ui::view_for(pickDark));
    be.bind_command(vm.step2->pickSolarized, wb::ios::ui::view_for(pickSolar));

    // Step 3: finish + result.
    [finishBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","finish").c_str()]
               forState:UIControlStateNormal];
    be.bind_command(vm.step3->finishCmd, wb::ios::ui::view_for(finishBtn));
    be.bind_text_oneway(vm.step3->finishedSummary, wb::ios::ui::view_for(resultLbl));
    return vc;
}

}  // namespace wb::wizard::iosview

namespace wb::wizard {
void register_wizard_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "wizard", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<WizardVmHostVm&>(vm);
            return iosview::build(host.inner(), host, be);
        });
}
}  // namespace wb::wizard
