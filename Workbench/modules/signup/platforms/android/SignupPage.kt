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
 *
 * Decomposed into sub-composables:
 *   FieldWithError — reusable field + error text pair (used for each input)
 *   SubmitSection  — submit button + result/error summary
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
        FieldWithError(vm, props, fieldKey = "username", labelKey = "username")
        FieldWithError(vm, props, fieldKey = "email", labelKey = "email")
        FieldWithError(vm, props, fieldKey = "password", labelKey = "password")
        FieldWithError(vm, props, fieldKey = "confirm", labelKey = "confirm")
        SubmitSection(vm, props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun FieldWithError(
    vm: AppViewModel,
    props: Map<String, String>,
    fieldKey: String,
    labelKey: String,
) {
    val error = props["$MOD.${fieldKey}_error"] ?: ""
    OutlinedTextField(
        value = props["$MOD.$fieldKey"] ?: "",
        onValueChange = { vm.setText(MOD, fieldKey, it) },
        label = { Text(props["$MOD.$labelKey"] ?: labelKey.replaceFirstChar { it.uppercase() }) },
        singleLine = true,
        modifier = Modifier.fillMaxWidth(),
        isError = error.isNotEmpty(),
    )
    Text(error, style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.error)
}

@Composable
private fun SubmitSection(vm: AppViewModel, props: Map<String, String>) {
    Button(onClick = { vm.execute(MOD, "submit") }, modifier = Modifier.fillMaxWidth()) { Text(props["$MOD.submit"] ?: "Submit") }
    Text(props["$MOD.submittedSummary"] ?: props["$MOD.unregistered"] ?: "", style = MaterialTheme.typography.bodyMedium)
}

// ── Module self-registration (Android twin of Qt/iOS register_<mod>_view) ──
fun register_signup_page() {
    com.dqsjqian.ariatools.ui.ComposeViewFactory.register("signup") { SignupPage(it) }
}
