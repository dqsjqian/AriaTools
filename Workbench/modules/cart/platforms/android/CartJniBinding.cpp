// ────────────────────────────────────────────────────────────────────────────
//  CartJniBinding.cpp — see CartJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/CartJniBinding.h"

namespace wb::cart {

void subscribe_cart(aria::runtime::EventBus& bus, CartVm& vm,
                    std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::bind_dbl;
    using wb::jni::bind_int;
    using wb::jni::push_property;

    bind_str(subs, "cart", "title", vm.title);
    bind_str(subs, "cart", "desc",  vm.desc);
    bind_str(subs, "cart", "draftName",  vm.draftName);
    bind_dbl(subs, "cart", "draftPrice", vm.draftPrice);
    bind_int(subs, "cart", "itemCount", vm.itemCount);
    bind_dbl(subs, "cart", "subtotal",  vm.subtotal);
    bind_dbl(subs, "cart", "tax",        vm.tax);
    bind_dbl(subs, "cart", "total",      vm.total);
    // Labels (i18n) — pulled from common i18n at VM construction.
    bind_str(subs, "cart", "name_label",  vm.nameLabel);
    bind_str(subs, "cart", "price_label", vm.priceLabel);
    bind_str(subs, "cart", "add",          vm.addLabel);
    bind_str(subs, "cart", "count",        vm.countLabel);
    bind_str(subs, "cart", "subtotal",     vm.subtotalLabel);
    bind_str(subs, "cart", "tax",           vm.taxLabel);
    bind_str(subs, "cart", "total",         vm.totalLabel);
    // Item list: push as newline-joined string.
    auto sync_items = [&vm]() {
        std::string joined;
        for (const auto& it : vm.items.snapshot()) {
            if (!joined.empty()) joined += "\n";
            joined += it->name() + " x" + std::to_string(it->qty_value());
        }
        push_property("cart", "items", joined);
    };
    sync_items();
    subs.push_back(vm.items.on_any_change(
        [sync_items]() { sync_items(); }));
}

void set_cart_text(CartVm& vm, const std::string& propName,
                   const std::string& value) {
    if (propName == "draftName") vm.draftName.set(value);
    else try { if (propName == "draftPrice") vm.draftPrice.set(std::stod(value)); } catch (...) {}
}

void exec_cart_command(CartVm& vm, const std::string& cmdName) {
    if (cmdName == "addItem") vm.addItem.execute();
}

void register_cart_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_cart(bus, static_cast<CartVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_cart_text(static_cast<CartVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_cart_command(static_cast<CartVm&>(vm), cmdName);
    };
    table.emplace("cart", b);
}

}  // namespace wb::cart
