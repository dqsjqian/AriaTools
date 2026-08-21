#include "FrameworkLabView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/FrameworkLabVm.h"

namespace wb::frameworklab {

void register_frameworklab_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "frameworklab",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::FrameworkLabView(
                static_cast<FrameworkLabVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::frameworklab
