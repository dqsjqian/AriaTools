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
 * ToolsPage — Android (Compose) view for the "tools" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun ToolsPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["tools.title"] ?: "tools", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["tools.desc"] ?: props["tools.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["tools.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("base64Output: ${props["tools.base64Output"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("randomOutput: ${props["tools.randomOutput"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("jsonOutput: ${props["tools.jsonOutput"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
