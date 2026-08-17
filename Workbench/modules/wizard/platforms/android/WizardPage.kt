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
 * WizardPage — Android (Compose) view for the "wizard" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun WizardPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["wizard.title"] ?: "wizard", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["wizard.desc"] ?: props["wizard.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["wizard.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("desc: ${props["wizard.desc"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("finishedSummary: ${props["wizard.finishedSummary"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("draftUsername: ${props["wizard.draftUsername"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("draftEmail: ${props["wizard.draftEmail"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("draftTheme: ${props["wizard.draftTheme"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
