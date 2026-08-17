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

private const val MOD = "tools"

/**
 * ToolsPage — Android (Compose) view for the "tools" module.
 * Three tool groups: Base64 encode/decode, random string, JSON format/minify.
 *
 * Decomposed into sub-composables:
 *   Base64Tool  — base64 input + encode/decode buttons + output
 *   RandomTool  — generate button + random output
 *   JsonTool    — json input + format/minify buttons + output
 */
@Composable
fun ToolsPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "tools", style = MaterialTheme.typography.headlineSmall)
        HorizontalDivider()
        Base64Tool(vm, props)
        HorizontalDivider()
        RandomTool(vm, props)
        HorizontalDivider()
        JsonTool(vm, props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun Base64Tool(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.base64_group"] ?: "Base64", style = MaterialTheme.typography.titleSmall)
    OutlinedTextField(
        value = props["$MOD.base64Input"] ?: "",
        onValueChange = { vm.setText(MOD, "base64Input", it) },
        label = { Text(props["$MOD.input"] ?: "Input") },
        modifier = Modifier.fillMaxWidth(),
    )
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Button(onClick = { vm.execute(MOD, "encodeBase64") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.encode"] ?: "Encode") }
        Button(onClick = { vm.execute(MOD, "decodeBase64") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.decode"] ?: "Decode") }
    }
    Text(props["$MOD.base64Output"] ?: "", style = MaterialTheme.typography.bodySmall)
}

@Composable
private fun RandomTool(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.random_group"] ?: "Random", style = MaterialTheme.typography.titleSmall)
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Button(onClick = { vm.execute(MOD, "generateRandom") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.generate"] ?: "Generate") }
    }
    Text(props["$MOD.randomOutput"] ?: "", style = MaterialTheme.typography.bodySmall)
}

@Composable
private fun JsonTool(vm: AppViewModel, props: Map<String, String>) {
    Text(props["$MOD.json_group"] ?: "JSON", style = MaterialTheme.typography.titleSmall)
    OutlinedTextField(
        value = props["$MOD.jsonInput"] ?: "",
        onValueChange = { vm.setText(MOD, "jsonInput", it) },
        label = { Text(props["$MOD.input"] ?: "Input") },
        modifier = Modifier.fillMaxWidth(),
    )
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Button(onClick = { vm.execute(MOD, "formatJson") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.format"] ?: "Format") }
        Button(onClick = { vm.execute(MOD, "minifyJson") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.minify"] ?: "Minify") }
    }
    Text(props["$MOD.jsonOutput"] ?: "", style = MaterialTheme.typography.bodySmall)
}
