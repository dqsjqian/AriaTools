package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_calendar_page() {
    ComposeViewFactory.register("calendar") { CalendarPage(it) }
}
