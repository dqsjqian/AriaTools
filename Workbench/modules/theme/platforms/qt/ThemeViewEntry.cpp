#include "ThemeView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ThemeVm.h"

namespace wb::theme {

void register_theme_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "theme",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::ThemeView>(static_cast<ThemeVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::theme
