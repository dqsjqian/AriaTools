#include "UnitConvertView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

void register_unitconvert_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "unitconvert", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::UnitConvertView>(static_cast<UnitConvertVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::unitconvert
