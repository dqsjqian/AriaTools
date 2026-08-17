package com.dqsjqian.ariatools.pages

import com.dqsjqian.ariatools.ui.ComposeViewFactory

fun register_notes_page() {
    ComposeViewFactory.register("notes") { NotesPage(it) }
}
