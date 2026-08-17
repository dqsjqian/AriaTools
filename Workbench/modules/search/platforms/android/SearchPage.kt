package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
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

private const val MOD = "search"

/**
 * SearchPage — Android (Compose) view for the "search" module.
 * Query input (debounced in VM) + debounced/distinct labels + history list.
 */
@Composable
fun SearchPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "search", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        OutlinedTextField(
            value = props["$MOD.query"] ?: "",
            onValueChange = { vm.setText(MOD, "query", it) },
            label = { Text(props["$MOD.placeholder"] ?: "Search") },
            singleLine = true,
            modifier = Modifier.fillMaxWidth(),
        )
        HorizontalDivider()
        Text("debounced: ${props["$MOD.debounced"] ?: ""}", style = MaterialTheme.typography.bodySmall)
        Text("distinct: ${props["$MOD.distinct"] ?: ""}", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        Text(props["$MOD.searches"] ?: "History", style = MaterialTheme.typography.titleSmall)
        val hits = (props["$MOD.hits"] ?: "").split('\n').filter { it.isNotBlank() }
        LazyColumn(modifier = Modifier.fillMaxWidth().weight(1f)) {
            items(hits) { h -> Text(h, style = MaterialTheme.typography.bodySmall) }
        }
    }
}
