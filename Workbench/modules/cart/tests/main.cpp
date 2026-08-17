#include "viewmodels/CartVm.h"
#include "events/CrossModuleEvents.h"

#include "aria/runtime/event_bus.hpp"

#include <cmath>
#include <cstdio>
#include <vector>

namespace {

int g_failures = 0;

void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("[cart-tests] FAIL: %s\n", what);
        ++g_failures;
    } else {
        std::printf("[cart-tests] ok: %s\n", what);
    }
}

bool near(double lhs, double rhs) {
    return std::abs(lhs - rhs) < 1e-9;
}

}  // namespace

int main() {
    aria::runtime::EventBus bus;
    std::vector<int> stateCounts;
    int qtyEvents = 0;
    int lastQty = 0;
    int orderEvents = 0;
    wb::shared::events::OrderPlaced lastOrder;

    auto stateSub = bus.subscribe<wb::shared::events::CartStateChanged>(
        [&](const auto& ev) { stateCounts.push_back(ev.itemCount); });
    auto qtySub = bus.subscribe<wb::shared::events::ItemQtyChanged>(
        [&](const auto& ev) { ++qtyEvents; lastQty = ev.newQty; });
    auto orderSub = bus.subscribe<wb::shared::events::OrderPlaced>(
        [&](const auto& ev) { ++orderEvents; lastOrder = ev; });

    wb::cart::CartVm vm(bus);
    check(!vm.checkout.can_execute(), "checkout initially disabled");
    check(near(vm.subtotal.get(), 0.0), "initial subtotal");
    check(vm.itemCount.get() == 0, "initial item count");

    vm.draftName.set("Apple");
    vm.draftPrice.set(3.5);
    vm.addItem.execute();
    check(vm.items.size() == 1, "add creates one row");
    check(near(vm.subtotal.get(), 3.5), "subtotal after add");
    check(near(vm.tax.get(), 0.28), "tax after add");
    check(near(vm.total.get(), 3.78), "total after add");
    check(vm.itemCount.get() == 1, "item count after add");
    check(stateCounts == std::vector<int>{1}, "add publishes full count");
    check(vm.checkout.can_execute(), "checkout enabled after add");

    vm.items.at(0)->qty().set(3);
    check(qtyEvents == 1 && lastQty == 3, "qty change forwarded with new value");
    check(near(vm.subtotal.get(), 10.5), "subtotal after qty change");
    check(near(vm.tax.get(), 0.84), "tax after qty change");
    check(near(vm.total.get(), 11.34), "total after qty change");
    check(vm.itemCount.get() == 3, "item count after qty change");
    check(stateCounts == std::vector<int>({1, 3}), "qty publishes full count");

    vm.items.remove_at(0);
    check(vm.itemCount.get() == 0 && near(vm.total.get(), 0.0),
          "single remove resets derived values");
    check(stateCounts == std::vector<int>({1, 3, 0}),
          "single remove publishes zero count");
    check(!vm.checkout.can_execute(), "checkout disabled after remove");

    vm.draftName.set("Pear");
    vm.draftPrice.set(2.0);
    vm.addItem.execute();
    vm.items.at(0)->qty().set(2);
    check(qtyEvents == 2 && lastQty == 2,
          "remaining item owns exactly one qty forwarding subscription");
    check(vm.itemCount.get() == 2 && near(vm.total.get(), 4.32),
          "second cart derived values before checkout");
    vm.checkout.execute();
    check(orderEvents == 1, "checkout publishes order");
    check(lastOrder.itemCount == 2 && near(lastOrder.total, 4.32),
          "order snapshot uses pre-clear derived values");
    check(vm.items.empty() && vm.itemCount.get() == 0,
          "checkout clears cart and derived count");
    check(stateCounts == std::vector<int>({1, 3, 0, 1, 2, 0}),
          "checkout publishes one final zero-count snapshot");
    check(!vm.checkout.can_execute(), "checkout disabled after checkout");

    std::puts(g_failures == 0 ? "[cart-tests] PASS" : "[cart-tests] FAIL");
    return g_failures == 0 ? 0 : 1;
}
