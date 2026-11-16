#include "SettingsView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SettingsVm.h"

namespace wb::settings {

void register_settings_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "settings",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::SettingsView>(static_cast<SettingsVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::settings
