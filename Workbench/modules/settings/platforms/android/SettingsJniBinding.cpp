// ────────────────────────────────────────────────────────────────────────────
//  SettingsJniBinding.cpp — see SettingsJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/SettingsJniBinding.h"

namespace wb::settings {

void subscribe_settings(aria::runtime::EventBus& bus, SettingsVm& vm,
                        std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "settings", "title",         vm.title);
    bind_str(subs, "settings", "hint",          vm.hint);
    bind_str(subs, "settings", "language",      vm.language);
    bind_str(subs, "settings", "languageLabel", vm.languageLabel);
}

void set_settings_text(SettingsVm& vm, const std::string& propName,
                       const std::string& value) {
    (void)vm;
    (void)propName;
    (void)value;  // Language switch goes through the command, not a property set.
}

void exec_settings_command(SettingsVm& vm, const std::string& cmdName) {
    if (cmdName == "switchLanguage-zh-CN") vm.switchLanguage.execute(std::string{"zh-CN"});
    else if (cmdName == "switchLanguage-en") vm.switchLanguage.execute(std::string{"en"});
}

void register_settings_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_settings(bus, static_cast<SettingsVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_settings_text(static_cast<SettingsVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_settings_command(static_cast<SettingsVm&>(vm), cmdName);
    };
    table.emplace("settings", b);
}

}  // namespace wb::settings
