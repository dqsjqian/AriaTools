package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_frameworklab_page() {
    ComposeViewFactory.register("frameworklab") { FrameworkLabPage(it) }
}
