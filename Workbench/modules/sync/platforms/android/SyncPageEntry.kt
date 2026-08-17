package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_sync_page() {
    ComposeViewFactory.register("sync") { SyncPage(it) }
}
