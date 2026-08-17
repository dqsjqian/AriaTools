// ────────────────────────────────────────────────────────────────────────────
//  LoginJniBinding.cpp — see LoginJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/LoginJniBinding.h"

namespace wb::login {

void subscribe_login(aria::runtime::EventBus& bus, LoginVm& vm,
                     std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::bind_bool;
    using wb::jni::bind_projected;

    bind_str(subs, "login", "title", vm.title);
    bind_str(subs, "login", "desc",  vm.desc);
    bind_str(subs, "login", "username", vm.username);
    bind_projected(subs, "login", "welcome", vm.login.last_result,
        [](const std::optional<LoginResult>& result) {
            return result ? result->welcome : std::string{};
        });
    bind_str(subs, "login", "error", vm.login.last_error_message);
    bind_bool(subs, "login", "is_executing", vm.login.is_executing);
}

void set_login_text(LoginVm& vm, const std::string& propName,
                    const std::string& value) {
    if (propName == "username") vm.username.set(value);
    else if (propName == "password") vm.password.set(value);
}

void exec_login_command(LoginVm& vm, const std::string& cmdName) {
    if (cmdName == "submit") vm.submitCmd.execute();
}

void register_login_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_login(bus, static_cast<LoginVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_login_text(static_cast<LoginVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_login_command(static_cast<LoginVm&>(vm), cmdName);
    };
    table.emplace("login", b);
}

}  // namespace wb::login
