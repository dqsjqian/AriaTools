#include "SettingsView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SettingsVm.h"

namespace wb::settings {

void register_settings_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "settings",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::SettingsView(static_cast<SettingsVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::settings
