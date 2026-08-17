#include "TipCalcView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/TipCalcVm.h"

namespace wb::tipcalc {

void register_tipcalc_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "tipcalc",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::TipCalcView(static_cast<TipCalcVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::tipcalc
