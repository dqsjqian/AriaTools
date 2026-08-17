// ────────────────────────────────────────────────────────────────────────────
//  UnitConvertJniBinding.cpp — see UnitConvertJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/UnitConvertJniBinding.h"

namespace wb::unitconvert {

void subscribe_unitconvert(aria::runtime::EventBus& bus, UnitConvertVm& vm,
                           std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::bind_dbl;

    bind_str(subs, "unitconvert", "title", vm.title);
    bind_str(subs, "unitconvert", "desc",  vm.desc);
    bind_dbl(subs, "unitconvert", "value",     vm.value);
    bind_dbl(subs, "unitconvert", "converted", vm.converted);
    // Labels (i18n).
    bind_str(subs, "unitconvert", "cat_temperature", vm.catTemperatureLabel);
    bind_str(subs, "unitconvert", "cat_length",      vm.catLengthLabel);
    bind_str(subs, "unitconvert", "cat_weight",      vm.catWeightLabel);
    bind_str(subs, "unitconvert", "input",           vm.inputLabel);
    bind_str(subs, "unitconvert", "equals",          vm.equalsLabel);
}

void set_unitconvert_text(UnitConvertVm& vm, const std::string& propName,
                          const std::string& value) {
    try {
        if (propName == "value") vm.value.set(std::stod(value));
    } catch (...) {}
}

void exec_unitconvert_command(UnitConvertVm& vm, const std::string& cmdName) {
    if (cmdName == "selectTemperature") vm.selectTemperature.execute();
    else if (cmdName == "selectLength")  vm.selectLength.execute();
    else if (cmdName == "selectWeight")  vm.selectWeight.execute();
}

void register_unitconvert_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_unitconvert(bus, static_cast<UnitConvertVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_unitconvert_text(static_cast<UnitConvertVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_unitconvert_command(static_cast<UnitConvertVm&>(vm), cmdName);
    };
    table.emplace("unitconvert", b);
}

}  // namespace wb::unitconvert
