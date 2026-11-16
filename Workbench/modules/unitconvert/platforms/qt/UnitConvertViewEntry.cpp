#include "UnitConvertView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

void register_unitconvert_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "unitconvert",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::UnitConvertView>(static_cast<UnitConvertVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::unitconvert
