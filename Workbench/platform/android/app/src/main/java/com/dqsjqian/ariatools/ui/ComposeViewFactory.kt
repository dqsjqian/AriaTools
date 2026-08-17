package com.dqsjqian.ariatools.ui

import androidx.compose.runtime.Composable
import com.dqsjqian.ariatools.AppViewModel

/**
 * ComposeViewFactory — module page registry for Android, symmetric with
 * QtViewFactory (Qt) / UIViewFactory (iOS). Each module registers its own
 * Compose page (from modules/<mod>/platforms/android/) keyed by module id;
 * AppRoot resolves the page for the current module instead of hard-coding
 * a when() dispatch.
 *
 * Registration lives in ModulePages.kt (the Android twin of the Qt/iOS
 * ViewManifest files).
 */
typealias ModulePage = @Composable (AppViewModel) -> Unit

object ComposeViewFactory {
    private val builders = mutableMapOf<String, ModulePage>()

    fun register(id: String, page: ModulePage) {
        builders[id] = page
    }

    fun build(id: String, vm: AppViewModel): (@Composable () -> Unit)? =
        builders[id]?.let { page -> ({ page(vm) }) }
}
