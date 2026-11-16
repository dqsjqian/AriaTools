#include "TipCalcView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/TipCalcVm.h"

namespace wb::tipcalc {

void register_tipcalc_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "tipcalc",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::TipCalcView>(static_cast<TipCalcVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::tipcalc
