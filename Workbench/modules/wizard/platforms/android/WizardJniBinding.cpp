// ────────────────────────────────────────────────────────────────────────────
//  WizardJniBinding.cpp — see WizardJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/WizardJniBinding.h"

namespace wb::wizard {

void subscribe_wizard(aria::runtime::EventBus& bus, WizardVmHostVm& vm,
                      std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "wizard", "title", vm.title);
    bind_str(subs, "wizard", "desc",  vm.desc);
    bind_str(subs, "wizard", "step1",    vm.step1Label);
    bind_str(subs, "wizard", "step2",    vm.step2Label);
    bind_str(subs, "wizard", "step3",    vm.step3Label);
    bind_str(subs, "wizard", "username", vm.usernameLabel);
    bind_str(subs, "wizard", "email",     vm.emailLabel);
    bind_str(subs, "wizard", "finish",    vm.finishLabel);
    bind_str(subs, "wizard", "unfinished",vm.unfinishedLabel);
    bind_str(subs, "wizard", "theme_light",     vm.themeLightLabel);
    bind_str(subs, "wizard", "theme_dark",      vm.themeDarkLabel);
    bind_str(subs, "wizard", "theme_solarized", vm.themeSolarizedLabel);
    bind_str(subs, "wizard", "finishedSummary", vm.inner().step3->finishedSummary);
    bind_str(subs, "wizard", "draftUsername", vm.inner().draft->username);
    bind_str(subs, "wizard", "draftEmail",    vm.inner().draft->email);
    bind_str(subs, "wizard", "draftTheme",     vm.inner().draft->theme);
}

void set_wizard_text(WizardVmHostVm& vm, const std::string& propName,
                     const std::string& value) {
    if (propName == "draftUsername") vm.inner().draft->username.set(value);
    else if (propName == "draftEmail") vm.inner().draft->email.set(value);
}

void exec_wizard_command(WizardVmHostVm& vm, const std::string& cmdName) {
    if (cmdName == "pickLight")     vm.inner().step2->pickLight.execute();
    else if (cmdName == "pickDark")  vm.inner().step2->pickDark.execute();
    else if (cmdName == "pickSolarized") vm.inner().step2->pickSolarized.execute();
    else if (cmdName == "finish")   vm.inner().step3->finishCmd.execute();
}

void register_wizard_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_wizard(bus, static_cast<WizardVmHostVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_wizard_text(static_cast<WizardVmHostVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_wizard_command(static_cast<WizardVmHostVm&>(vm), cmdName);
    };
    table.emplace("wizard", b);
}

}  // namespace wb::wizard
