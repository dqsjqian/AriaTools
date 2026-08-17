package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_search_page() {
    ComposeViewFactory.register("search") { SearchPage(it) }
}
