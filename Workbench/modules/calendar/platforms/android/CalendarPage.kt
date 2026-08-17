package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

private const val MOD = "calendar"

/**
 * CalendarPage — Android (Compose) view for the "calendar" module.
 *
 * Decomposed into sub-composables (mirroring Qt/iOS sub-views):
 *   MonthNavRow   — prev/today/next buttons + month title
 *   SubscriptionBar — URL input + subscribe + refresh buttons
 *   EventList     — events list (from VM days snapshot)
 *
 * Each sub-composable is a small, testable unit; CalendarPage assembles
 * them into a vertical column and wires the shared AppViewModel.
 */
@Composable
fun CalendarPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        // Header
        Text(props["$MOD.title"] ?: "calendar", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.hint"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()

        // Sub-views
        MonthNavRow(vm, props)
        SubscriptionBar(vm, props)
        HorizontalDivider()

        // Status + event list
        Text(props["$MOD.status"] ?: "", style = MaterialTheme.typography.bodyMedium)
        EventList(props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun MonthNavRow(vm: AppViewModel, props: Map<String, String>) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Button(onClick = { vm.execute(MOD, "prevMonth") }, modifier = Modifier.weight(1f)) {
            Text(props["$MOD.prev"] ?: "Prev")
        }
        Text(
            props["$MOD.monthTitle"] ?: "",
            style = MaterialTheme.typography.titleMedium,
            modifier = Modifier.weight(2f).padding(vertical = 4.dp),
        )
        Button(onClick = { vm.execute(MOD, "today") }, modifier = Modifier.weight(1f)) {
            Text(props["$MOD.today"] ?: "Today")
        }
        Button(onClick = { vm.execute(MOD, "nextMonth") }, modifier = Modifier.weight(1f)) {
            Text(props["$MOD.next"] ?: "Next")
        }
    }
}

@Composable
private fun SubscriptionBar(vm: AppViewModel, props: Map<String, String>) {
    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
        OutlinedTextField(
            value = props["$MOD.subscribeUrl"] ?: "",
            onValueChange = { vm.setText(MOD, "subscribeUrl", it) },
            label = { Text(props["$MOD.url_placeholder"] ?: "ICS URL") },
            singleLine = true,
            modifier = Modifier.fillMaxWidth(),
        )
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            Button(onClick = { vm.execute(MOD, "addSubscription") }, modifier = Modifier.weight(1f)) {
                Text(props["$MOD.subscribe"] ?: "Subscribe")
            }
            Button(onClick = { vm.execute(MOD, "refresh") }, modifier = Modifier.weight(1f)) {
                Text(props["$MOD.refresh"] ?: "Refresh")
            }
        }
    }
}

@Composable
private fun EventList(props: Map<String, String>) {
    val events = (props["$MOD.events"] ?: "").split('\n').filter { it.isNotBlank() }
    LazyColumn(
        modifier = Modifier.fillMaxWidth(),
        verticalArrangement = Arrangement.spacedBy(2.dp),
    ) {
        items(events) { e -> Text(e, style = MaterialTheme.typography.bodySmall) }
    }
}

// ── Module self-registration (Android twin of Qt/iOS register_<mod>_view) ──
fun register_calendar_page() {
    com.dqsjqian.ariatools.ui.ComposeViewFactory.register("calendar") { CalendarPage(it) }
}
