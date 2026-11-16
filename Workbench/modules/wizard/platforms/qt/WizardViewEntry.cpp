#include "WizardView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/WizardVmHostVm.h"

namespace wb::wizard {

void register_wizard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "wizard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::WizardView>(static_cast<WizardVmHostVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::wizard
