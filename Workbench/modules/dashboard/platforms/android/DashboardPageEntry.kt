package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_dashboard_page() {
    ComposeViewFactory.register("dashboard") { DashboardPage(it) }
}
