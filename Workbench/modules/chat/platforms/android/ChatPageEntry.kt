package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_chat_page() {
    ComposeViewFactory.register("chat") { ChatPage(it) }
}
