#include "LoginView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/LoginVm.h"

namespace wb::login {

void register_login_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "login",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::LoginView(static_cast<LoginVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::login
