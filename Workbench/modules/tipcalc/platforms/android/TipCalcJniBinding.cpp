// ────────────────────────────────────────────────────────────────────────────
//  TipCalcJniBinding.cpp — see TipCalcJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/TipCalcJniBinding.h"

namespace wb::tipcalc {

void subscribe_tipcalc(aria::runtime::EventBus& bus, TipCalcVm& vm,
                       std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::bind_dbl;
    using wb::jni::bind_int;

    bind_str(subs, "tipcalc", "title", vm.title);
    bind_str(subs, "tipcalc", "desc",  vm.desc);
    bind_dbl(subs, "tipcalc", "bill",       vm.bill);
    bind_int(subs, "tipcalc", "tipPercent", vm.tipPercent);
    bind_int(subs, "tipcalc", "people",     vm.people);
    bind_dbl(subs, "tipcalc", "tipAmount",  vm.tipAmount);
    bind_dbl(subs, "tipcalc", "total",      vm.total);
    bind_dbl(subs, "tipcalc", "perPerson",  vm.perPerson);
    // Labels (i18n).
    bind_str(subs, "tipcalc", "bill_label",   vm.billLabel);
    bind_str(subs, "tipcalc", "tip_label",    vm.tipLabel);
    bind_str(subs, "tipcalc", "people_label", vm.peopleLabel);
    bind_str(subs, "tipcalc", "tip_amount",   vm.tipAmountText);
    bind_str(subs, "tipcalc", "total",        vm.totalText);
    bind_str(subs, "tipcalc", "per_person",   vm.perPersonText);
    bind_str(subs, "tipcalc", "round_up",     vm.roundUpText);
}

void set_tipcalc_text(TipCalcVm& vm, const std::string& propName,
                      const std::string& value) {
    // Numeric inputs arrive as strings; parse to the underlying type.
    try {
        if (propName == "bill")       vm.bill.set(std::stod(value));
        else if (propName == "tipPercent") vm.tipPercent.set(std::stoi(value));
        else if (propName == "people")     vm.people.set(std::stoi(value));
    } catch (...) { /* ignore malformed numeric input */ }
}

void exec_tipcalc_command(TipCalcVm& vm, const std::string& cmdName) {
    if (cmdName == "roundUp") vm.roundUp.execute();
}

void register_tipcalc_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_tipcalc(bus, static_cast<TipCalcVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_tipcalc_text(static_cast<TipCalcVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_tipcalc_command(static_cast<TipCalcVm&>(vm), cmdName);
    };
    table.emplace("tipcalc", b);
}

}  // namespace wb::tipcalc
