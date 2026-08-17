package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
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
 * Cross-module navigation demo: buttons fire the VM's Commands; the routing
 * decision (which module AND how to present it) lives in the C++ VM layer.
 * This page renders the pushed page by module id + presentation kind pushed
 * over the JNI side-channel:
 *   0 = Push   -> embedded below the buttons
 *   1 = Modal  -> Compose Dialog overlay
 *   2 = Window -> mobile falls back to a full-screen Dialog
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

        // ── Cross-module navigation (VM-layer routing) ──────────────────
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "openCart") }) {
                Text(props["$MOD.open_cart"] ?: "Open cart")
            }
            Button(onClick = { vm.execute(MOD, "modalCart") }) {
                Text(props["$MOD.modal_cart"] ?: "Open cart (modal)")
            }
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "windowCart") }) {
                Text(props["$MOD.window_cart"] ?: "Open cart (window)")
            }
            Button(onClick = { vm.execute(MOD, "navBack") }) {
                Text(props["$MOD.back"] ?: "Back")
            }
        }

        // Render the pushed page by module id + presentation kind.
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
                        modifier = Modifier.fillMaxWidth().padding(16.dp),
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
