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
 */
@Composable
fun DashboardPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(props["dashboard.welcome"] ?: "", style = MaterialTheme.typography.headlineSmall)
        Text(props["dashboard.summary"] ?: "", style = MaterialTheme.typography.bodyLarge)
    }
}
