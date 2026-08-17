package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
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
 * Real interaction: month navigation (prev/next/today), ICS subscription
 * URL input + subscribe button, refresh button, and the day grid + event
 * list rendered from VM state pushed over JNI.
 *
 * Symmetric with the Qt CalendarView (QListWidget grid + QLineEdits +
 * QPushButtons) and the iOS CalendarView (UILabels + UITextFields +
 * UIButtons).
 */
@Composable
fun CalendarPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "calendar", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.hint"] ?: "", style = MaterialTheme.typography.bodySmall)

        // Month title + nav buttons
        Text(props["$MOD.monthTitle"] ?: "", style = MaterialTheme.typography.titleMedium)
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            Button(onClick = { vm.execute(MOD, "prevMonth") }, modifier = Modifier.weight(1f)) {
                Text(props["$MOD.prev"] ?: "Prev")
            }
            Button(onClick = { vm.execute(MOD, "today") }, modifier = Modifier.weight(1f)) {
                Text(props["$MOD.today"] ?: "Today")
            }
            Button(onClick = { vm.execute(MOD, "nextMonth") }, modifier = Modifier.weight(1f)) {
                Text(props["$MOD.next"] ?: "Next")
            }
        }

        // Subscription
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

        HorizontalDivider()

        // Status + event list (pushed as newline-joined string from C++).
        Text(props["$MOD.status"] ?: "", style = MaterialTheme.typography.bodyMedium)
        val events = (props["$MOD.events"] ?: "").split('\n').filter { it.isNotBlank() }
        LazyColumn(
            modifier = Modifier.fillMaxWidth().weight(1f),
            verticalArrangement = Arrangement.spacedBy(2.dp),
        ) {
            items(events) { e -> Text(e, style = MaterialTheme.typography.bodySmall) }
        }
    }
}
