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
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

private const val MOD = "settings"

/**
 * SettingsPage — Android (Compose) view for the "settings" module.
 * Language selector: two buttons switch the UI language via VM command.
 */
@Composable
fun SettingsPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(props["$MOD.title"] ?: "settings", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.hint"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        Text(props["$MOD.language"] ?: "Language", style = MaterialTheme.typography.titleMedium)
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "switchLanguage-zh-CN") }, modifier = Modifier.weight(1f)) { Text("简体中文") }
            Button(onClick = { vm.execute(MOD, "switchLanguage-en") }, modifier = Modifier.weight(1f)) { Text("English") }
        }
    }
}
