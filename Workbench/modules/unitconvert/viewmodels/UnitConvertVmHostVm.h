#pragma once
//
// UnitConvertVmHostVm — adapts the plain (non-ViewModel) UnitConvertVm to the
// IModule contract (ViewModel lifecycle) by composition.
//
#include "module_api/BaseVm.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

class UnitConvertVmHostVm final : public wb::core::BaseVm {
public:
    UnitConvertVmHostVm() {
        text(title, "title");
        text(desc,  "desc");
        text(catTemperatureLabel, "cat_temperature");
        text(catLengthLabel,      "cat_length");
        text(catWeightLabel,      "cat_weight");
        text(inputLabel,          "input");
        text(equalsLabel,         "equals");
    }

    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    // Category picker labels (i18n).
    aria::Property<std::string> catTemperatureLabel;
    aria::Property<std::string> catLengthLabel;
    aria::Property<std::string> catWeightLabel;
    aria::Property<std::string> inputLabel;
    aria::Property<std::string> equalsLabel;
    UnitConvertVm& inner() { return vm_; }
    const UnitConvertVm& inner() const { return vm_; }
private:
    UnitConvertVm vm_;
};

}  // namespace wb::unitconvert
