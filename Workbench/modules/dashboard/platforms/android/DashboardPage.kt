package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import com.dqsjqian.ariatools.AppViewModel
import com.dqsjqian.ariatools.ui.ComposeViewFactory

private const val MOD = "dashboard"

/**
 * DashboardPage — Android (Compose) view for the "dashboard" module.
 *
 * Demos two decoupled capabilities, both driven by the C++ VM layer over
 * the JNI side-channel:
 *
 * 1. Extension point (mount): cart provides UI for slot "dashboard.content"
 *    via MountRegistry; this page shows the mount state and lets the user
 *    toggle it on/off. Full provider-UI rendering is demonstrated on
 *    Qt/iOS (the side-channel here mirrors only the state, not the mounted
 *    VM's inner properties).
 *
 * 2. Navigation: modal/window presentation kinds render as Compose Dialog.
 */
@Composable
fun DashboardPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        WelcomeText(props)
        SummaryText(props)

        // ── Extension point (mount) state ───────────────────────────────
        val mounted = props["$MOD.mountedModule"] ?: ""
        Text(
            props["$MOD.mount_status"] ?: "",
            style = MaterialTheme.typography.bodySmall,
            color = MaterialTheme.colorScheme.primary,
        )

        // ── Buttons: navigation + extension toggle ──────────────────────
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "modalCart") }) {
                Text(props["$MOD.modal_cart"] ?: "Open cart (modal)")
            }
            Button(onClick = { vm.execute(MOD, "windowCart") }) {
                Text(props["$MOD.window_cart"] ?: "Open cart (window)")
            }
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "navBack") }) {
                Text(props["$MOD.back"] ?: "Back")
            }
            Button(onClick = { vm.execute(MOD, "mountToggle") }) {
                Text(props["$MOD.mount_toggle"] ?: "Toggle extension")
            }
        }

        // Render the pushed navigation page (modal/window kinds as Dialog).
        val navModule = props["$MOD.navCurrentModule"] ?: ""
        val depth = props["$MOD.navDepth"] ?: "0"
        val navPres = (props["$MOD.navPresentation"] ?: "0").toIntOrNull() ?: 0
        Text(
            "nav depth = $depth, presentation = $navPres",
            style = MaterialTheme.typography.bodySmall,
        )
        HorizontalDivider()

        val page = ComposeViewFactory.build(navModule, vm)
        if (page != null) {
            if (navPres == 1 || navPres == 2) {
                // Modal / Window: overlay. Dismissing the dialog pops the
                // stack entry (same semantics as Qt QDialog / iOS present).
                Dialog(onDismissRequest = { vm.execute(MOD, "navBack") }) {
                    Surface(
                        modifier = Modifier.padding(16.dp),
                        shape = MaterialTheme.shapes.medium,
                        color = MaterialTheme.colorScheme.surface,
                    ) {
                        Column(modifier = Modifier.padding(16.dp)) {
                            page()
                        }
                    }
                }
            } else {
                // Push: embed in place.
                page()
            }
        }
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun WelcomeText(props: Map<String, String>) {
    Text(props["$MOD.welcome"] ?: "", style = MaterialTheme.typography.headlineSmall)
}

@Composable
private fun SummaryText(props: Map<String, String>) {
    Text(props["$MOD.summary"] ?: "", style = MaterialTheme.typography.bodyLarge)
}
