#pragma once
//
// LoginVm — Tab 6: simulated login
//
// Framework features
//   - binding::ViewModel         is_active / on_activate / on_deactivate
//   - binding::ViewModelScope    cancels every in-flight coroutine on
//                                VM destruction via scope.cancel()
//   - AsyncCommand<R, Args...>   three-state Property fan-out plus
//                                built-in cancellation support
//

#include "aria/aria.hpp"
#include "aria/command.hpp"
#include "module_api/BaseVm.h"
#include "aria/async/async_command.hpp"
#include "aria/async/executor.hpp"
#include "aria/async/task.hpp"
#include "aria/binding/view_model.hpp"
#include "aria/binding/view_model_scope.hpp"

#include <string>

namespace wb::login {

struct LoginResult {
    std::string welcome;
    friend bool operator==(const LoginResult& a, const LoginResult& b) noexcept {
        return a.welcome == b.welcome;
    }
};

class LoginVm : public wb::core::BaseVm {
public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    LoginVm(aria::async::IExecutor& ui, aria::async::IExecutor& worker);

    aria::Property<std::string> username{"alice"};
    aria::Property<std::string> password{""};

    aria::async::AsyncCommand<LoginResult, std::string, std::string> login;

    /// Parameterless command wrapper around submit(); lets every platform
    /// drive the login button via the same bind_command surface.
    aria::Command<> submitCmd{[this]{ submit(); }};

    /// Reject submission while the VM is suspended.
    void submit();

private:
    aria::binding::ViewModelScope scope_;
};

}  // namespace wb::login
