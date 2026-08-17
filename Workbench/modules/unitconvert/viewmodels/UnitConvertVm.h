#pragma once
//
// UnitConvertVm — Tab 2: unit converter
//
// Scenario
//   Type a value + switch the category (Temperature / Length / Weight)
//   → automatic conversion.
//   Inside the Computed a switch picks the formula based on category.
//   Switching the category causes the Computed to rebuild its upstream
//   dependency set on the next recompute ("conditional dependencies").
//
// The Computed lambdas capture `this` and read the enum. To keep the
// header self-contained we leave them inline; moving them into the .cpp
// would force splitting them into free functions + forwarding, with
// marginal benefit.
//

#include "aria/aria.hpp"
#include "aria/command.hpp"

#include <string>

namespace wb::unitconvert {

enum class Category { Temperature, Length, Weight };

class UnitConvertVm {
public:
    aria::Property<Category> category{Category::Temperature};
    aria::Property<double>   value{25.0};

    /// Category picker commands — each selects a fixed category. This
    /// keeps the VM enum-typed while letting every platform drive it
    /// via the same command surface (UIButton / ComboBox→command /
    /// Compose button). CanExecute is always true.
    aria::Command<> selectTemperature{[this]{ category.set(Category::Temperature); }};
    aria::Command<> selectLength     {[this]{ category.set(Category::Length);      }};
    aria::Command<> selectWeight     {[this]{ category.set(Category::Weight);      }};

    aria::Computed<std::string> fromLabel{[this] {
        switch (category.get()) {
            case Category::Temperature: return std::string("°C");
            case Category::Length:      return std::string("m");
            case Category::Weight:      return std::string("kg");
        }
        return std::string("?");
    }};

    aria::Computed<std::string> toLabel{[this] {
        switch (category.get()) {
            case Category::Temperature: return std::string("°F");
            case Category::Length:      return std::string("ft");
            case Category::Weight:      return std::string("lb");
        }
        return std::string("?");
    }};

    aria::Computed<double> converted{[this] {
        const double x = value.get();
        switch (category.get()) {
            case Category::Temperature: return x * 9.0 / 5.0 + 32.0;
            case Category::Length:      return x * 3.28084;
            case Category::Weight:      return x * 2.20462;
        }
        return 0.0;
    }};
};

}  // namespace wb::unitconvert
