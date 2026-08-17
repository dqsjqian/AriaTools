package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_theme_page() {
    ComposeViewFactory.register("theme") { ThemePage(it) }
}
