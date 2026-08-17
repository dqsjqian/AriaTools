package com.dqsjqian.ariatools.ui

import com.dqsjqian.ariatools.pages.register_calendar_page
import com.dqsjqian.ariatools.pages.register_cart_page
import com.dqsjqian.ariatools.pages.register_chat_page
import com.dqsjqian.ariatools.pages.register_dashboard_page
import com.dqsjqian.ariatools.pages.register_login_page
import com.dqsjqian.ariatools.pages.register_notes_page
import com.dqsjqian.ariatools.pages.register_search_page
import com.dqsjqian.ariatools.pages.register_settings_page
import com.dqsjqian.ariatools.pages.register_signup_page
import com.dqsjqian.ariatools.pages.register_sync_page
import com.dqsjqian.ariatools.pages.register_theme_page
import com.dqsjqian.ariatools.pages.register_tipcalc_page
import com.dqsjqian.ariatools.pages.register_tools_page
import com.dqsjqian.ariatools.pages.register_unitconvert_page
import com.dqsjqian.ariatools.pages.register_wizard_page

/**
 * ModulePages — Android page registration manifest, the twin of
 * QtViewManifest.cpp / IosViewManifest.mm. Called once at startup; every
 * module self-registers its Compose page into ComposeViewFactory via its
 * own register_<mod>_page() function (defined in each module's Page.kt).
 *
 * To add a module: create modules/<mod>/platforms/android/<Mod>Page.kt with
 * a register_<mod>_page() function, add one import + one call here.
 */
fun registerAllModulePages() {
    register_dashboard_page()
    register_notes_page()
    register_calendar_page()
    register_tools_page()
    register_settings_page()
    register_sync_page()
    register_tipcalc_page()
    register_unitconvert_page()
    register_cart_page()
    register_signup_page()
    register_search_page()
    register_login_page()
    register_chat_page()
    register_theme_page()
    register_wizard_page()
}
