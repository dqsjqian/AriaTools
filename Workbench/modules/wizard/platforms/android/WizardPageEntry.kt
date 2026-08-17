package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_wizard_page() {
    ComposeViewFactory.register("wizard") { WizardPage(it) }
}
