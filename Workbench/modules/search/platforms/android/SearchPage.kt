package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

/**
 * SearchPage — Android (Compose) view for the "search" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun SearchPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["search.title"] ?: "search", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["search.desc"] ?: props["search.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["search.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("desc: ${props["search.desc"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("query: ${props["search.query"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("debounced: ${props["search.debounced"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("distinct: ${props["search.distinct"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
