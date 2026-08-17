package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_login_page() {
    ComposeViewFactory.register("login") { LoginPage(it) }
}
