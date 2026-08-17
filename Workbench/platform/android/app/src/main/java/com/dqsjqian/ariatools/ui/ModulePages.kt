package com.dqsjqian.ariatools.ui

import com.dqsjqian.ariatools.pages.CalendarPage
import com.dqsjqian.ariatools.pages.CartPage
import com.dqsjqian.ariatools.pages.ChatPage
import com.dqsjqian.ariatools.pages.DashboardPage
import com.dqsjqian.ariatools.pages.LoginPage
import com.dqsjqian.ariatools.pages.NotesPage
import com.dqsjqian.ariatools.pages.SearchPage
import com.dqsjqian.ariatools.pages.SettingsPage
import com.dqsjqian.ariatools.pages.SignupPage
import com.dqsjqian.ariatools.pages.SyncPage
import com.dqsjqian.ariatools.pages.ThemePage
import com.dqsjqian.ariatools.pages.TipCalcPage
import com.dqsjqian.ariatools.pages.ToolsPage
import com.dqsjqian.ariatools.pages.UnitConvertPage
import com.dqsjqian.ariatools.pages.WizardPage

/**
 * ModulePages — Android page registration manifest, the twin of
 * QtViewManifest.cpp / IosViewManifest.mm. Called once at startup; every
 * module self-registers its Compose page into ComposeViewFactory.
 */
fun registerAllModulePages() {
    ComposeViewFactory.register("dashboard") { DashboardPage(it) }
    ComposeViewFactory.register("notes") { NotesPage(it) }
    ComposeViewFactory.register("calendar") { CalendarPage(it) }
    ComposeViewFactory.register("tools") { ToolsPage(it) }
    ComposeViewFactory.register("settings") { SettingsPage(it) }
    ComposeViewFactory.register("sync") { SyncPage(it) }
    ComposeViewFactory.register("tipcalc") { TipCalcPage(it) }
    ComposeViewFactory.register("unitconvert") { UnitConvertPage(it) }
    ComposeViewFactory.register("cart") { CartPage(it) }
    ComposeViewFactory.register("signup") { SignupPage(it) }
    ComposeViewFactory.register("search") { SearchPage(it) }
    ComposeViewFactory.register("login") { LoginPage(it) }
    ComposeViewFactory.register("chat") { ChatPage(it) }
    ComposeViewFactory.register("theme") { ThemePage(it) }
    ComposeViewFactory.register("wizard") { WizardPage(it) }
}
