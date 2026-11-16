#include "LoginView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/LoginVm.h"

namespace wb::login {

void register_login_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "login",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::LoginView>(static_cast<LoginVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::login
