#include "module/CartModule.h"
#include "viewmodels/CartVm.h"

#include "module_api/capabilities/cart/ICartPage.h"

namespace wb::cart {

std::shared_ptr<aria::binding::ViewModel>
CartModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        return std::make_shared<CartVm>(ctx.bus());
}

void CartModule::register_navigation(wb::module_api::NavigatorHost& nav) {
    // Cart is a navigation target for ICartPage. The factory creates a
    // *fresh* CartVm per push; params are delivered via on_navigate.
    nav.Register<wb::module_api::ICartPage>(
        id(),
        [](wb::module_api::ModuleContext& ctx) {
            return std::make_shared<CartVm>(ctx.bus());
        });
}

void CartModule::register_mounts(wb::module_api::MountRegistry& mounts) {
    // Cart provides UI for the dashboard's "content" extension point. The
    // dashboard never includes cart headers — it only knows the slot id;
    // cart never knows who the host is. Fresh CartVm per resolve.
    mounts.Provide(wb::module_api::slots::kDashboardContent, id(),
                   [](wb::module_api::ModuleContext& ctx) {
                       return std::make_shared<CartVm>(ctx.bus());
                   });
}

std::shared_ptr<wb::module_api::IModule> make_cart_module() {
    return std::make_shared<CartModule>();
}

}  // namespace wb::cart
