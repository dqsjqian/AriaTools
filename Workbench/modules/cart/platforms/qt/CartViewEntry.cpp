#include "CartView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/CartVm.h"

namespace wb::cart {

void register_cart_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "cart",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::CartView>(static_cast<CartVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::cart
