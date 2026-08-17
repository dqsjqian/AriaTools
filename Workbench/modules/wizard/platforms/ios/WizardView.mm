#include "WizardView.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/WizardVm.h"
#include "viewmodels/WizardVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::wizard::iosview {

// ─── Step1View ─────────────────────────────────────────────────────────────
Step1View::Step1View(Step1Vm& vm, aria::binding::BindingEngine& be)
    : view_([[UIStackView alloc] init]) {
    ((UIStackView*)view_).axis = UILayoutConstraintAxisVertical;
    ((UIStackView*)view_).spacing = 8;
    ((UIStackView*)view_).distribution = UIStackViewDistributionFill;

    UILabel*     step1Lbl = wb::ios::ui::make_label(@"");
    UITextField* userField = wb::ios::ui::make_field(@"");
    UITextField* mailField = wb::ios::ui::make_field(@"");

    [(UIStackView*)view_ addArrangedSubview:step1Lbl];
    [(UIStackView*)view_ addArrangedSubview:userField];
    [(UIStackView*)view_ addArrangedSubview:mailField];

    // Step label (i18n).
    step1Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step1").c_str()];

    // Step 1: username + email.
    userField.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","username").c_str()];
    mailField.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","email").c_str()];
    be.bind_text(vm.draft->username, wb::ios::ui::view_for(userField));
    be.bind_text(vm.draft->email,    wb::ios::ui::view_for(mailField));
}

// ─── Step2View ─────────────────────────────────────────────────────────────
Step2View::Step2View(Step2Vm& vm, aria::binding::BindingEngine& be)
    : view_([[UIStackView alloc] init]) {
    ((UIStackView*)view_).axis = UILayoutConstraintAxisVertical;
    ((UIStackView*)view_).spacing = 8;
    ((UIStackView*)view_).distribution = UIStackViewDistributionFill;

    UILabel*  step2Lbl = wb::ios::ui::make_label(@"");
    UIButton* pickLight = wb::ios::ui::make_button(@"");
    UIButton* pickDark  = wb::ios::ui::make_button(@"");
    UIButton* pickSolar = wb::ios::ui::make_button(@"");

    [(UIStackView*)view_ addArrangedSubview:step2Lbl];
    [(UIStackView*)view_ addArrangedSubview:pickLight];
    [(UIStackView*)view_ addArrangedSubview:pickDark];
    [(UIStackView*)view_ addArrangedSubview:pickSolar];

    // Step label (i18n).
    step2Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step2").c_str()];

    // Step 2: theme picker — three buttons drive Step2Vm commands.
    [pickLight setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_light").c_str()]
              forState:UIControlStateNormal];
    [pickDark  setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_dark").c_str()]
              forState:UIControlStateNormal];
    [pickSolar setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","theme_solarized").c_str()]
              forState:UIControlStateNormal];
    be.bind_command(vm.pickLight,     wb::ios::ui::view_for(pickLight));
    be.bind_command(vm.pickDark,      wb::ios::ui::view_for(pickDark));
    be.bind_command(vm.pickSolarized, wb::ios::ui::view_for(pickSolar));
}

// ─── Step3View ─────────────────────────────────────────────────────────────
Step3View::Step3View(Step3Vm& vm, aria::binding::BindingEngine& be)
    : view_([[UIStackView alloc] init]) {
    ((UIStackView*)view_).axis = UILayoutConstraintAxisVertical;
    ((UIStackView*)view_).spacing = 8;
    ((UIStackView*)view_).distribution = UIStackViewDistributionFill;

    UILabel*  step3Lbl = wb::ios::ui::make_label(@"");
    UIButton* finishBtn = wb::ios::ui::make_button(@"");
    UILabel*  resultLbl = wb::ios::ui::make_label(@"");

    [(UIStackView*)view_ addArrangedSubview:step3Lbl];
    [(UIStackView*)view_ addArrangedSubview:finishBtn];
    [(UIStackView*)view_ addArrangedSubview:resultLbl];

    // Step label (i18n).
    step3Lbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("wizard","step3").c_str()];

    // Step 3: finish + result.
    [finishBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("wizard","finish").c_str()]
               forState:UIControlStateNormal];
    be.bind_command(vm.finishCmd, wb::ios::ui::view_for(finishBtn));
    be.bind_text_oneway(vm.finishedSummary, wb::ios::ui::view_for(resultLbl));
}

// ─── Top-level WizardView ──────────────────────────────────────────────────
WizardView::WizardView(WizardVmHostVm& host, aria::binding::BindingEngine& be)
    : vc_(nil) {
    auto& vm = host.inner();

    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* desc  = wb::ios::ui::make_label(@"");

    Step1View step1(*vm.step1, be);
    Step2View step2(*vm.step2, be);
    Step3View step3(*vm.step3, be);

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, desc, step1.view(), step2.view(), step3.view()]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));
}

}  // namespace wb::wizard::iosview
