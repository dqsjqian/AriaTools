#pragma once
//
// CartModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::cart {

class CartModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "cart"; }
    std::string nav_key() const override { return "nav_cart"; }
    int order() const override { return 8; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;

    /// Registers the cart page as a cross-module navigation target
    /// (ICartPage). Other modules navigate here via
    /// `ctx.navigator().Push<ICartPage>(params)`.
    void register_navigation(wb::module_api::NavigatorHost& nav) override;
};

}  // namespace wb::cart

namespace wb::module_api { class IModule; }
namespace wb::cart {
std::shared_ptr<wb::module_api::IModule> make_cart_module();
}
