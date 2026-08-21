#include "FrameworkLabView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/FrameworkLabVm.h"

namespace wb::frameworklab {

void register_frameworklab_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "frameworklab",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::FrameworkLabView>(
                static_cast<FrameworkLabVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::frameworklab
