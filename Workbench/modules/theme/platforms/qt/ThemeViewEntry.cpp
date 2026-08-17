#include "ThemeView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ThemeVm.h"

namespace wb::theme {

void register_theme_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "theme",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::ThemeView(static_cast<ThemeVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::theme
