// ────────────────────────────────────────────────────────────────────────────
//  ThemeJniBinding.cpp — see ThemeJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/ThemeJniBinding.h"

namespace wb::theme {

void subscribe_theme(aria::runtime::EventBus& bus, ThemeVm& vm,
                     std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "theme", "title", vm.title);
    bind_str(subs, "theme", "desc",  vm.desc);
    bind_str(subs, "theme", "currentId",          vm.currentId);
    bind_str(subs, "theme", "currentDisplayName", vm.currentDisplayName);
    // Theme picker labels (i18n).
    bind_str(subs, "theme", "theme_light",     vm.themeLightLabel);
    bind_str(subs, "theme", "theme_dark",      vm.themeDarkLabel);
    bind_str(subs, "theme", "theme_solarized", vm.themeSolarizedLabel);
    bind_str(subs, "theme", "card_title",      vm.cardTitleLabel);
    bind_str(subs, "theme", "card_body",       vm.cardBodyLabel);
}

void set_theme_text(ThemeVm& vm, const std::string& propName,
                    const std::string& value) {
    (void)vm;
    (void)propName;
    (void)value;  // Theme picked via command, not property set.
}

void exec_theme_command(ThemeVm& vm, const std::string& cmdName) {
    if (cmdName == "pickLight")      vm.pickLight.execute();
    else if (cmdName == "pickDark")  vm.pickDark.execute();
    else if (cmdName == "pickSolarized") vm.pickSolarized.execute();
}

void register_theme_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_theme(bus, static_cast<ThemeVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_theme_text(static_cast<ThemeVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_theme_command(static_cast<ThemeVm&>(vm), cmdName);
    };
    table.emplace("theme", b);
}

}  // namespace wb::theme
