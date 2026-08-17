#include "SignupView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SignupVm.h"

namespace wb::signup {

void register_signup_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "signup",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::SignupView(static_cast<SignupVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::signup
