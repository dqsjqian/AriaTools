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
 * LoginPage — Android (Compose) view for the "login" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun LoginPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["login.title"] ?: "login", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["login.desc"] ?: props["login.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["login.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("desc: ${props["login.desc"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("username: ${props["login.username"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("welcome: ${props["login.welcome"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("error: ${props["login.error"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("is_executing: ${props["login.is_executing"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
