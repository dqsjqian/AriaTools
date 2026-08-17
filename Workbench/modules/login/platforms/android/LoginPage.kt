package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
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

private const val MOD = "login"

/**
 * LoginPage — Android (Compose) view for the "login" module.
 * Username + password form + login button (executes async login) +
 * spinner (visible while is_executing) + welcome/error labels.
 *
 * Decomposed into sub-composables:
 *   LoginForm    — username/password fields + login button + spinner
 *   LoginStatus  — welcome label + conditional error text
 */
@Composable
fun LoginPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    val busy = (props["$MOD.is_executing"] ?: "0") == "1"
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "login", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        LoginForm(vm, props, busy)
        LoginStatus(props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun LoginForm(vm: AppViewModel, props: Map<String, String>, busy: Boolean) {
    OutlinedTextField(value = props["$MOD.username"] ?: "", onValueChange = { vm.setText(MOD, "username", it) }, label = { Text(props["$MOD.username"] ?: "Username") }, singleLine = true, modifier = Modifier.fillMaxWidth())
    OutlinedTextField(value = props["$MOD.password"] ?: "", onValueChange = { vm.setText(MOD, "password", it) }, label = { Text(props["$MOD.password"] ?: "Password") }, singleLine = true, modifier = Modifier.fillMaxWidth())
    Button(onClick = { vm.execute(MOD, "submit") }, enabled = !busy, modifier = Modifier.fillMaxWidth()) {
        Text(if (busy) (props["$MOD.logging_in"] ?: "Logging in…") else (props["$MOD.login"] ?: "Login"))
    }
    if (busy) CircularProgressIndicator(modifier = Modifier.padding(8.dp))
}

@Composable
private fun LoginStatus(props: Map<String, String>) {
    Text(props["$MOD.welcome"] ?: (props["$MOD.not_logged_in"] ?: ""), style = MaterialTheme.typography.bodyMedium)
    if ((props["$MOD.error"] ?: "").isNotEmpty()) {
        Text(props["$MOD.error"] ?: "", style = MaterialTheme.typography.bodyMedium, color = MaterialTheme.colorScheme.error)
    }
}
