#include "UnitConvertView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

void register_unitconvert_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "unitconvert",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::UnitConvertView(static_cast<UnitConvertVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::unitconvert
