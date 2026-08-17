package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

private const val MOD = "dashboard"

/**
 * DashboardPage — Android (Compose) view for the "dashboard" module.
 * Read-only overview: welcome + summary from VM (i18n, auto-refresh).
 *
 * Decomposed into sub-composables:
 *   WelcomeText  — headline welcome label
 *   SummaryText  — body summary paragraph
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

// ── Module self-registration (Android twin of Qt/iOS register_<mod>_view) ──
fun register_dashboard_page() {
    com.dqsjqian.ariatools.ui.ComposeViewFactory.register("dashboard") { DashboardPage(it) }
}
