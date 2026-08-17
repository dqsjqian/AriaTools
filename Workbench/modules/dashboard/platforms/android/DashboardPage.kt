package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel
import com.dqsjqian.ariatools.ui.ComposeViewFactory

private const val MOD = "dashboard"

/**
 * DashboardPage — Android (Compose) view for the "dashboard" module.
 *
 * Cross-module navigation demo: the "open cart" button fires the VM's
 * openCart Command; the routing decision lives in the C++ VM layer
 * (Navigator::push). This page merely renders the pushed page by module id
 * from the props pushed over the JNI side-channel.
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
            Button(onClick = { vm.execute(MOD, "navBack") }) {
                Text(props["$MOD.back"] ?: "Back")
            }
        }

        // Render the pushed page (module id pushed over the side-channel).
        val navModule = props["$MOD.navCurrentModule"] ?: ""
        val depth = props["$MOD.navDepth"] ?: "0"
        Text(
            "nav depth = $depth",
            style = MaterialTheme.typography.bodySmall,
        )
        HorizontalDivider()
        if (navModule.isNotEmpty()) {
            Text(
                "opened: $navModule",
                style = MaterialTheme.typography.bodySmall,
            )
            ComposeViewFactory.build(navModule, vm)?.let { page ->
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
