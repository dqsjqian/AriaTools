#include "TipCalcView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/TipCalcVm.h"

namespace wb::tipcalc {

void register_tipcalc_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "tipcalc", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::TipCalcView>(static_cast<TipCalcVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::tipcalc
