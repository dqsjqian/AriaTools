#include "app/ModulesManifest.h"

// 各模块工厂声明（模块自导出）。加模块 = 这里加一行 include + 一行 add。
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
