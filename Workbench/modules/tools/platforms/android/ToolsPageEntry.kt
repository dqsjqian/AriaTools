package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_tools_page() {
    ComposeViewFactory.register("tools") { ToolsPage(it) }
}
