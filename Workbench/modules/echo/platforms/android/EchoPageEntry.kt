package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_echo_page() {
    ComposeViewFactory.register("echo") { EchoPage(it) }
}
