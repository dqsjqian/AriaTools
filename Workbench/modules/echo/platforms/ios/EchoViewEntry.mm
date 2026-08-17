#include "EchoView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/EchoVm.h"

namespace wb::echo {

void register_echo_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "echo", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::EchoView>(static_cast<EchoVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::echo
