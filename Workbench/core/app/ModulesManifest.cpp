#include "app/ModulesManifest.h"

// Each module's factory declaration (module self-exports). Add a module = add one include + one add line.
#include "module/DashboardModule.h"
#include "module/NotesModule.h"
#include "module/CalendarModule.h"
#include "module/ToolsModule.h"
#include "module/SettingsModule.h"
#include "module/SyncModule.h"
#include "module/TipcalcModule.h"
#include "module/UnitConvertModule.h"
#include "module/CartModule.h"
#include "module/SignupModule.h"
#include "module/SearchModule.h"
#include "module/LoginModule.h"
#include "module/ChatModule.h"
#include "module/ThemeModule.h"
#include "module/WizardModule.h"

namespace wb::app {

void populate_modules(wb::module_api::ModuleRegistry& registry) {
    registry.add(wb::dashboard::make_dashboard_module());
    registry.add(wb::notes::make_notes_module());
    registry.add(wb::calendar::make_calendar_module());
    registry.add(wb::tools::make_tools_module());
    registry.add(wb::settings::make_settings_module());
    registry.add(wb::sync::make_sync_module());
    registry.add(wb::tipcalc::make_tipcalc_module());
    registry.add(wb::unitconvert::make_unitconvert_module());
    registry.add(wb::cart::make_cart_module());
    registry.add(wb::signup::make_signup_module());
    registry.add(wb::search::make_search_module());
    registry.add(wb::login::make_login_module());
    registry.add(wb::chat::make_chat_module());
    registry.add(wb::theme::make_theme_module());
    registry.add(wb::wizard::make_wizard_module());
}

}  // namespace wb::app
