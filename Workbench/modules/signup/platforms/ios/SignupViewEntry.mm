#include "SignupView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SignupVm.h"

namespace wb::signup {

void register_signup_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "signup", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::SignupView>(static_cast<SignupVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::signup
