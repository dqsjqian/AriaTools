#include "ThemeView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/ThemeVm.h"

namespace wb::theme {

void register_theme_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "theme", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::ThemeView>(static_cast<ThemeVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::theme
