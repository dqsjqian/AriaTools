package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_cart_page() {
    ComposeViewFactory.register("cart") { CartPage(it) }
}
