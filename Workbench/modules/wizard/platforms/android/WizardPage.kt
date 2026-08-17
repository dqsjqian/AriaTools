package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
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

private const val MOD = "wizard"

/**
 * WizardPage — Android (Compose) view for the "wizard" module.
 * Three-step signup wizard: Step1 (account), Step2 (theme), Step3 (finish).
 * All three steps are laid out vertically for the small-screen form factor.
 *
 * Decomposed into sub-composables:
 *   Step1Section — account fields (draft username + draft email)
 *   Step2Section — theme picker buttons (light/dark/solarized)
 *   Step3Section — finish button + finished/unfinished summary
 */
@Composable
fun WizardPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "wizard", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        Step1Section(vm, props)
        HorizontalDivider()
        Step2Section(vm, props)
        HorizontalDivider()
        Step3Section(vm, props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun Step1Section(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.step1"] ?: "Step 1", style = MaterialTheme.typography.titleSmall)
    OutlinedTextField(value = props["$MOD.draftUsername"] ?: "", onValueChange = { vm.setText(MOD, "draftUsername", it) }, label = { Text(props["$MOD.username"] ?: "Username") }, singleLine = true, modifier = Modifier.fillMaxWidth())
    OutlinedTextField(value = props["$MOD.draftEmail"] ?: "", onValueChange = { vm.setText(MOD, "draftEmail", it) }, label = { Text(props["$MOD.email"] ?: "Email") }, singleLine = true, modifier = Modifier.fillMaxWidth())
}

@Composable
private fun Step2Section(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.step2"] ?: "Step 2", style = MaterialTheme.typography.titleSmall)
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Button(onClick = { vm.execute(MOD, "pickLight") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_light"] ?: "Light") }
        Button(onClick = { vm.execute(MOD, "pickDark") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_dark"] ?: "Dark") }
        Button(onClick = { vm.execute(MOD, "pickSolarized") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_solarized"] ?: "Solarized") }
    }
}

@Composable
private fun Step3Section(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.step3"] ?: "Step 3", style = MaterialTheme.typography.titleSmall)
    Button(onClick = { vm.execute(MOD, "finish") }, modifier = Modifier.fillMaxWidth()) { Text(props["$MOD.finish"] ?: "Finish") }
    Text(props["$MOD.finishedSummary"] ?: (props["$MOD.unfinished"] ?: ""), style = MaterialTheme.typography.bodyMedium)
}

// ── Module self-registration (Android twin of Qt/iOS register_<mod>_view) ──
fun register_wizard_page() {
    com.dqsjqian.ariatools.ui.ComposeViewFactory.register("wizard") { WizardPage(it) }
}
