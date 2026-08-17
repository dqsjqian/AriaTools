package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

/**
 * DashboardPage — Android (Compose) view for the "dashboard" module.
 * Renders the C++ VM's welcome/summary properties pushed over JNI.
 */
@Composable
fun DashboardPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    val welcome = props["dashboard.welcome"] ?: ""
    val summary = props["dashboard.summary"] ?: ""

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        Text("Dashboard", style = MaterialTheme.typography.headlineSmall)
        HorizontalDivider(modifier = Modifier.padding(vertical = 8.dp))
        Text(welcome, style = MaterialTheme.typography.titleMedium)
        Spacer(Modifier.height(12.dp))
        Text(summary, style = MaterialTheme.typography.bodyLarge)
    }
}
