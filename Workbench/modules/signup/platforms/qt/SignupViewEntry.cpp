#include "SignupView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SignupVm.h"

namespace wb::signup {

void register_signup_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "signup",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::SignupView>(static_cast<SignupVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::signup
