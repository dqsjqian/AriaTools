package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_settings_page() {
    ComposeViewFactory.register("settings") { SettingsPage(it) }
}
