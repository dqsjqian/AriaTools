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

private const val MOD = "sync"

/**
 * SyncPage — Android (Compose) view for the "sync" module.
 * Git sync config form (data dir, remote, branch, user, token) + action buttons.
 */
@Composable
fun SyncPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "sync", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.hint"] ?: "", style = MaterialTheme.typography.bodySmall)
        Text(props["$MOD.status"] ?: "", style = MaterialTheme.typography.bodyMedium)
        HorizontalDivider()
        OutlinedTextField(value = props["$MOD.dataDir"] ?: "", onValueChange = { vm.setText(MOD, "dataDir", it) }, label = { Text(props["$MOD.data_dir"] ?: "Data dir") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.remote"] ?: "", onValueChange = { vm.setText(MOD, "remote", it) }, label = { Text(props["$MOD.remote_label"] ?: "Remote") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.branch"] ?: "", onValueChange = { vm.setText(MOD, "branch", it) }, label = { Text(props["$MOD.branch_label"] ?: "Branch") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.username"] ?: "", onValueChange = { vm.setText(MOD, "username", it) }, label = { Text(props["$MOD.username"] ?: "Username") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.token"] ?: "", onValueChange = { vm.setText(MOD, "token", it) }, label = { Text(props["$MOD.token"] ?: "Token") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "saveConfig") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.save_config"] ?: "Save") }
            Button(onClick = { vm.execute(MOD, "syncNow") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.sync_now"] ?: "Sync") }
        }
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "pull") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.pull"] ?: "Pull") }
            Button(onClick = { vm.execute(MOD, "push") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.push"] ?: "Push") }
        }
    }
}
