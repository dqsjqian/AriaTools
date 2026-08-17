package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
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

private const val MOD = "signup"

/**
 * SignupPage — Android (Compose) view for the "signup" module.
 * Four-field form (username/email/password/confirm) with per-field error
 * hints + form-level error + submit button + result summary.
 */
@Composable
fun SignupPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(6.dp),
    ) {
        Text(props["$MOD.title"] ?: "signup", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        OutlinedTextField(value = props["$MOD.username"] ?: "", onValueChange = { vm.setText(MOD, "username", it) }, label = { Text(props["$MOD.username"] ?: "Username") }, singleLine = true, modifier = Modifier.fillMaxWidth(), isError = (props["$MOD.username_error"] ?: "").isNotEmpty())
        Text(props["$MOD.username_error"] ?: "", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.error)
        OutlinedTextField(value = props["$MOD.email"] ?: "", onValueChange = { vm.setText(MOD, "email", it) }, label = { Text(props["$MOD.email"] ?: "Email") }, singleLine = true, modifier = Modifier.fillMaxWidth(), isError = (props["$MOD.email_error"] ?: "").isNotEmpty())
        Text(props["$MOD.email_error"] ?: "", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.error)
        OutlinedTextField(value = props["$MOD.password"] ?: "", onValueChange = { vm.setText(MOD, "password", it) }, label = { Text(props["$MOD.password"] ?: "Password") }, singleLine = true, modifier = Modifier.fillMaxWidth(), isError = (props["$MOD.password_error"] ?: "").isNotEmpty())
        Text(props["$MOD.password_error"] ?: "", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.error)
        OutlinedTextField(value = props["$MOD.confirm"] ?: "", onValueChange = { vm.setText(MOD, "confirm", it) }, label = { Text(props["$MOD.confirm"] ?: "Confirm") }, singleLine = true, modifier = Modifier.fillMaxWidth(), isError = (props["$MOD.confirm_error"] ?: "").isNotEmpty())
        Text(props["$MOD.confirm_error"] ?: "", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.error)
        Button(onClick = { vm.execute(MOD, "submit") }, modifier = Modifier.fillMaxWidth()) { Text(props["$MOD.submit"] ?: "Submit") }
        Text(props["$MOD.submittedSummary"] ?: props["$MOD.unregistered"] ?: "", style = MaterialTheme.typography.bodyMedium)
    }
}
