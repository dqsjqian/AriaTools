#include "viewmodels/LoginVm.h"

#include "infra/i18n/I18n.h"

#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>

namespace wb::login {

namespace {

// Compute on the worker; resolve the final message on the graph/UI thread
// using the language selected when the operation completes.
aria::async::Task<LoginResult> fake_login(wb::i18n::Context context,
                                          aria::async::IExecutor& ui,
                                          std::string u, std::string p) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> d(0, 99);
    const char* error = p.empty() ? "err_password_empty"
                      : u == "bob" ? "err_user_banned"
                      : d(rng) < 40 ? "err_invalid_credentials" : nullptr;
    co_await aria::async::schedule_on(ui);
    if (error) throw std::runtime_error(context->tr("login", error));
    co_return LoginResult{context->tr("login", "welcome_prefix") + u + "!"};
}

}  // namespace

LoginVm::LoginVm(wb::i18n::Context context, aria::async::IExecutor& ui, aria::async::IExecutor& worker)
    : BaseVm(std::move(context)), login(ui, worker, [locale = i18n_context(), &ui](std::string u, std::string p) {
          return fake_login(locale, ui, std::move(u), std::move(p));
      })
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
