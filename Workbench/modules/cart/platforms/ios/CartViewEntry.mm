#include "CartView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/CartVm.h"

namespace wb::cart {

void register_cart_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "cart", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::CartView>(static_cast<CartVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::cart
