package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_unitconvert_page() {
    ComposeViewFactory.register("unitconvert") { UnitConvertPage(it) }
}
