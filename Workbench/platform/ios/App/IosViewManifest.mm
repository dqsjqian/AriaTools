#include "App/IosViewManifest.h"

namespace wb::dashboard { void register_dashboard_view(); }
namespace wb::notes     { void register_notes_view(); }
namespace wb::calendar  { void register_calendar_view(); }
namespace wb::tools     { void register_tools_view(); }
namespace wb::settings  { void register_settings_view(); }
namespace wb::sync      { void register_sync_view(); }

namespace wb::ios {

void register_all_views() {
    wb::dashboard::register_dashboard_view();
    wb::notes::register_notes_view();
    wb::calendar::register_calendar_view();
    wb::tools::register_tools_view();
    wb::settings::register_settings_view();
    wb::sync::register_sync_view();
}

}  // namespace wb::ios
