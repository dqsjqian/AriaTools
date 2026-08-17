package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_signup_page() {
    ComposeViewFactory.register("signup") { SignupPage(it) }
}
