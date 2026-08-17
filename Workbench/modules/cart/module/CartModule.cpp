#include "module/CartModule.h"
#include "viewmodels/CartVm.h"

namespace wb::cart {

std::shared_ptr<aria::binding::ViewModel>
CartModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        (void)ctx; return std::make_shared<CartVm>();
}

std::shared_ptr<wb::module_api::IModule> make_cart_module() {
    return std::make_shared<CartModule>();
}

}  // namespace wb::cart
