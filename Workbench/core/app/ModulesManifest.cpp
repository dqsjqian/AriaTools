#include "app/ModulesManifest.h"

// Each module's factory declaration (module self-exports). Add a module = add one include + one add line.
#include "module/DashboardModule.h"
#include "module/NotesModule.h"
#include "module/CalendarModule.h"
#include "module/ToolsModule.h"
#include "module/SettingsModule.h"
#include "module/SyncModule.h"

namespace wb::app {

void populate_modules(wb::module_api::ModuleRegistry& registry) {
    registry.add(wb::dashboard::make_dashboard_module());
    registry.add(wb::notes::make_notes_module());
    registry.add(wb::calendar::make_calendar_module());
    registry.add(wb::tools::make_tools_module());
    registry.add(wb::settings::make_settings_module());
    registry.add(wb::sync::make_sync_module());
}

}  // namespace wb::app
