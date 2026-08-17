#include "LoginView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/LoginVm.h"

namespace wb::login {

void register_login_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "login", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::LoginView>(static_cast<LoginVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::login
