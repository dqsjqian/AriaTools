#include "viewmodels/LoginVm.h"

#include "infra/i18n/I18n.h"

#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>

namespace wb::login {

namespace {

/// Build the localized error message for a given key at call time.
/// Async tasks run on the worker thread; resolving the localized string
/// at throw time (not at VM construction) keeps the message in sync with
/// the currently selected UI language.
[[nodiscard]] std::string err(const char* key) {
    return wb::i18n::str_in("login", key);
}

aria::async::Task<LoginResult> fake_login(std::string u, std::string p) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> d(0, 99);
    if (p.empty())   throw std::runtime_error(err("err_password_empty"));
    if (u == "bob")  throw std::runtime_error(err("err_user_banned"));
    if (d(rng) < 40) throw std::runtime_error(err("err_invalid_credentials"));
    // Welcome message: localized prefix + username.
    co_return LoginResult{wb::i18n::str_in("login", "welcome_prefix") + u + "!"};
}

}  // namespace

LoginVm::LoginVm(aria::async::IExecutor& ui, aria::async::IExecutor& worker)
    : login(ui, worker, &fake_login)
{
    text(title, "title");
    text(desc, "desc");
    scope_.attach(*this);
}

void LoginVm::submit() {
    if (!is_active().get()) return;
    login.execute(username.get(), password.get());
}

}  // namespace wb::login
