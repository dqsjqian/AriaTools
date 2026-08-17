// dashboard navigation tests — cross-module routing lives in the VM layer,
// typed through framework-kernel interfaces (no business headers needed).
// Two INDEPENDENT payload channels are exercised:
//   1. typed struct:  Push<ICartPage>(CartArgs{...}) → on_navigate(const CartArgs&)
//   2. raw json:      Push<ICartPage>(json{...})     → on_navigate(const json&)
// Plus the graceful-degradation contract for unregistered target interfaces.
#include "app/AppCore.h"
#include "module_api/NavigationEntryVm.h"
#include "module_api/NavigatorHost.h"
#include "module_api/capabilities/cart/ICartPage.h"
#include "viewmodels/DashboardVm.h"

#include "module/DashboardModule.h"
#include "module/CartModule.h"
#include "module_api/ModuleContext.h"
#include "module_api/ModuleRegistry.h"

#include "viewmodels/CartVm.h"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <filesystem>
#include <string>

namespace {

int g_failures = 0;

void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("[dashboard-tests] FAIL: %s\n", what);
        ++g_failures;
    } else {
        std::printf("[dashboard-tests] ok: %s\n", what);
    }
}

/// Read the pushed CartVm from the navigation stack top.
wb::cart::CartVm* top_cart(wb::module_api::NavigatorHost& nav) {
    auto* entry = dynamic_cast<wb::module_api::NavigationEntryVm*>(
        nav.current().get().get());
    if (!entry) return nullptr;
    return dynamic_cast<wb::cart::CartVm*>(&entry->inner());
}

}  // namespace

int main() {
    using namespace wb::dashboard;
    using namespace wb::module_api;

    // A temp i18n dir is enough: BaseVm::text falls back to [module/key]
    // when files are missing, which the VM tolerates.
    auto tmp = std::filesystem::temp_directory_path() / "wb-dashboard-tests";
    std::filesystem::remove_all(tmp);
    std::filesystem::create_directories(tmp);

    wb::infra::ServiceHub hub(tmp.string());
    ModuleContext ctx(hub);

    // Navigator + registry: only dashboard + cart are needed here.
    auto navigator = std::make_shared<NavigatorHost>(ctx);
    ctx.set_navigator(navigator);
    ModuleRegistry reg;
    reg.add(wb::dashboard::make_dashboard_module());
    reg.add(wb::cart::make_cart_module());
    ctx.set_vm_factory([&](const std::string& id) {
        for (const auto& m : reg.all()) {
            if (m->id() == id) return m->create_view_model(ctx);
        }
        return std::shared_ptr<aria::binding::ViewModel>{};
    });
    // Modules register their navigation targets (as AppCore::load_modules does).
    for (const auto& m : reg.all()) {
        m->register_navigation(*navigator);
    }

    auto vm = std::static_pointer_cast<DashboardVm>(
        wb::dashboard::make_dashboard_module()->create_view_model(ctx));

    check(vm->navigator().current().get() == nullptr, "nav root is empty");
    check(vm->navDepth.get() == 0, "nav depth 0 at root");
    check(vm->navCurrentModule.get().empty(), "navCurrentModule empty at root");

    // ── Channel 1: typed struct (dashboard openCart) ───────────────────
    vm->openCart.execute();
    auto* entry = dynamic_cast<NavigationEntryVm*>(
        vm->navigator().current().get().get());
    check(entry != nullptr, "current is a NavigationEntryVm");
    check(entry && entry->module_id() == "cart", "entry targets cart module");
    check(vm->navDepth.get() == 1, "nav depth 1 after push");
    check(vm->navCurrentModule.get() == "cart", "navCurrentModule mirrors cart");
    check(entry && entry->inner().is_active().get(), "pushed cart VM is active");

    auto* cart = top_cart(*navigator);
    check(cart != nullptr, "pushed entry inner is CartVm");
    check(cart && cart->draftName.get() == "Apple",
          "typed channel prefilled draft name (CartArgs.product)");
    check(cart && cart->draftPrice.get() == 2.5,
          "typed channel prefilled draft price (CartArgs.price)");

    // ── Channel 2: raw json (free-form, caller/consumer independent) ──
    // The caller sends MORE fields than the consumer needs ("note" is
    // ignored by CartVm) — json payloads are not tied to any struct shape.
    check(navigator->Push<ICartPage>(
              nlohmann::json{{"product", "Pear"}, {"price", 1.5}, {"note", "from dashboard"}}),
          "raw-json Push succeeds");
    check(vm->navDepth.get() == 2, "nav depth 2 after json push");
    auto* cart2 = top_cart(*navigator);
    check(cart2 && cart2->draftName.get() == "Pear",
          "json channel prefilled draft name (parsed from json)");
    check(cart2 && cart2->draftPrice.get() == 1.5,
          "json channel prefilled draft price (parsed from json)");

    // ── navBack: pop returns to the previous entry ─────────────────────
    check(vm->navBack.can_execute(), "navBack enabled while stacked");
    vm->navBack.execute();
    check(vm->navDepth.get() == 1, "nav depth 1 after pop");
    check(vm->navCurrentModule.get() == "cart", "first cart page visible again");

    vm->navBack.execute();
    check(vm->navDepth.get() == 0, "nav depth 0 back at root");
    check(vm->navigator().current().get() == nullptr, "nav root empty after pops");

    // ── Graceful degradation: unregistered target interface ────────────
    struct UnregisteredTarget : INavigationTarget {
        bool on_navigate(const nlohmann::json&) override { return false; }
    };
    check(!navigator->Push<UnregisteredTarget>(nlohmann::json{}),
          "Push to unregistered target returns false");
    check(vm->navDepth.get() == 0, "failed push leaves stack untouched");

    // ── Success signalling: target that does NOT implement a channel ───
    // A target registering ICartPage but only handling json: pushing a typed
    // CartArgs must FAIL (target reports not consumed), so the caller knows
    // its payload was not delivered — not a silent no-op push.
    class JsonOnlyCart : public aria::binding::ViewModel,
                         public ICartPage {
    public:
        bool on_navigate(const nlohmann::json& payload) override {
            lastProduct_ = payload.value(ICartPage::kParamProduct, std::string{});
            return true;
        }
        std::string lastProduct_;
    };
    navigator->Register<ICartPage>(
        "jsoncart",
        [](ModuleContext&) -> std::shared_ptr<aria::binding::ViewModel> {
            return std::static_pointer_cast<aria::binding::ViewModel>(
                std::make_shared<JsonOnlyCart>());
        });
    check(navigator->Push<ICartPage>(nlohmann::json{{"product", "Kiwi"}}),
          "json push to json-only target succeeds");
    check(vm->navDepth.get() == 1, "json-only push landed on stack");
    check(navigator->Pop(), "pop json-only page");
    check(!navigator->Push<ICartPage>(CartArgs{.product = "Mango", .price = 9.0}),
          "typed push to json-only target returns false (not consumed)");
    check(vm->navDepth.get() == 0, "unconsumed typed push leaves stack untouched");

    std::filesystem::remove_all(tmp);

    std::puts(g_failures == 0 ? "[dashboard-tests] PASS" : "[dashboard-tests] FAIL");
    return g_failures == 0 ? 0 : 1;
}
