#include "WizardView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/WizardVmHostVm.h"

namespace wb::wizard {

void register_wizard_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "wizard", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::WizardView>(static_cast<WizardVmHostVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::wizard
