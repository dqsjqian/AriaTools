#include "SettingsView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SettingsVm.h"

namespace wb::settings {

void register_settings_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "settings", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::SettingsView>(static_cast<SettingsVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::settings
