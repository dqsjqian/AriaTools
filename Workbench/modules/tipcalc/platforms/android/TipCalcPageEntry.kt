package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_tipcalc_page() {
    ComposeViewFactory.register("tipcalc") { TipCalcPage(it) }
}
