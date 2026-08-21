package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
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

private const val MOD = "frameworklab"

@Composable
fun FrameworkLabPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    val tasks = (props["$MOD.tasks"] ?: "").split('\n').filter { it.isNotBlank() }
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        OutlinedTextField(
            value = props["$MOD.draft"] ?: "",
            onValueChange = { vm.setText(MOD, "draft", it) },
            label = { Text(props["$MOD.draftLabel"] ?: "") },
            modifier = Modifier.fillMaxWidth(),
            singleLine = true,
        )
        Button(onClick = { vm.execute(MOD, "addTask") }, modifier = Modifier.fillMaxWidth()) {
            Text(props["$MOD.addLabel"] ?: "")
        }
        LazyColumn(modifier = Modifier.weight(1f).fillMaxWidth()) {
            itemsIndexed(tasks) { index, task ->
                Text(
                    task,
                    modifier = Modifier.fillMaxWidth().clickable {
                        vm.setText(MOD, "selectedIndex", index.toString())
                    }.padding(8.dp),
                )
            }
        }
        Text(props["$MOD.selected"] ?: "")
        Text(props["$MOD.summary"] ?: "")
        Row(horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            Button(onClick = { vm.execute(MOD, "toggleSelected") }) {
                Text(props["$MOD.toggleLabel"] ?: "")
            }
            Button(onClick = { vm.execute(MOD, "removeSelected") }) {
                Text(props["$MOD.removeLabel"] ?: "")
            }
        }
        Button(onClick = { vm.execute(MOD, "clearCompleted") }, modifier = Modifier.fillMaxWidth()) {
            Text(props["$MOD.clearLabel"] ?: "")
        }
        HorizontalDivider()
        Text(props["$MOD.graphLabel"] ?: "", style = MaterialTheme.typography.titleSmall)
        Button(onClick = { vm.execute(MOD, "refreshGraph") }) {
            Text(props["$MOD.refreshLabel"] ?: "")
        }
        Text(props["$MOD.graph"] ?: "", style = MaterialTheme.typography.bodySmall)
    }
}
