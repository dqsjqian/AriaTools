// ────────────────────────────────────────────────────────────────────────────
//  SignupJniBinding.cpp — see SignupJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/SignupJniBinding.h"

namespace wb::signup {

void subscribe_signup(aria::runtime::EventBus& bus, SignupVm& vm,
                      std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "signup", "title", vm.title);
    bind_str(subs, "signup", "desc",  vm.desc);
    bind_str(subs, "signup", "submittedSummary", vm.submittedSummary);
    // Per-field error messages.
    bind_str(subs, "signup", "username_error", vm.username.error);
    bind_str(subs, "signup", "email_error",    vm.email.error);
    bind_str(subs, "signup", "password_error", vm.password.error);
    bind_str(subs, "signup", "confirm_error",  vm.confirm.error);
}

void set_signup_text(SignupVm& vm, const std::string& propName,
                     const std::string& value) {
    if (propName == "username") vm.username.value.set(value);
    else if (propName == "email")    vm.email.value.set(value);
    else if (propName == "password") vm.password.value.set(value);
    else if (propName == "confirm")  vm.confirm.value.set(value);
}

void exec_signup_command(SignupVm& vm, const std::string& cmdName) {
    if (cmdName == "submit") vm.submit.execute();
}

void register_signup_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_signup(bus, static_cast<SignupVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_signup_text(static_cast<SignupVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_signup_command(static_cast<SignupVm&>(vm), cmdName);
    };
    table.emplace("signup", b);
}

}  // namespace wb::signup
