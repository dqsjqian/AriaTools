#include "App/IosViewManifest.h"

namespace wb::dashboard { void register_dashboard_view(); }
namespace wb::notes     { void register_notes_view(); }
namespace wb::calendar  { void register_calendar_view(); }
namespace wb::tools     { void register_tools_view(); }
namespace wb::settings  { void register_settings_view(); }
namespace wb::sync      { void register_sync_view(); }
namespace wb::tipcalc   { void register_tipcalc_view(); }
namespace wb::cart       { void register_cart_view(); }
namespace wb::chat       { void register_chat_view(); }
namespace wb::login      { void register_login_view(); }
namespace wb::search     { void register_search_view(); }
namespace wb::signup     { void register_signup_view(); }
namespace wb::theme      { void register_theme_view(); }
namespace wb::unitconvert { void register_unitconvert_view(); }
namespace wb::wizard      { void register_wizard_view(); }

namespace wb::ios {

void register_all_views() {
    wb::dashboard::register_dashboard_view();
    wb::notes::register_notes_view();
    wb::calendar::register_calendar_view();
    wb::tools::register_tools_view();
    wb::settings::register_settings_view();
    wb::sync::register_sync_view();
    wb::tipcalc::register_tipcalc_view();
    wb::cart::register_cart_view();
    wb::chat::register_chat_view();
    wb::login::register_login_view();
    wb::search::register_search_view();
    wb::signup::register_signup_view();
    wb::theme::register_theme_view();
    wb::unitconvert::register_unitconvert_view();
    wb::wizard::register_wizard_view();
}

}  // namespace wb::ios
