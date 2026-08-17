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
 * SyncPage — Android (Compose) view for the "sync" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun SyncPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["sync.title"] ?: "sync", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["sync.desc"] ?: props["sync.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["sync.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("hint: ${props["sync.hint"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("status: ${props["sync.status"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
