package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
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

private const val MOD = "cart"

/**
 * CartPage — Android (Compose) view for the "cart" module.
 * Add-item form (name + price + add button) + item list + totals.
 */
@Composable
fun CartPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "cart", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        OutlinedTextField(value = props["$MOD.draftName"] ?: "", onValueChange = { vm.setText(MOD, "draftName", it) }, label = { Text(props["$MOD.name_label"] ?: "Name") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.draftPrice"] ?: "", onValueChange = { vm.setText(MOD, "draftPrice", it) }, label = { Text(props["$MOD.price_label"] ?: "Price") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        Button(onClick = { vm.execute(MOD, "addItem") }, modifier = Modifier.fillMaxWidth()) { Text(props["$MOD.add"] ?: "Add") }
        HorizontalDivider()
        val items = (props["$MOD.items"] ?: "").split('\n').filter { it.isNotBlank() }
        LazyColumn(modifier = Modifier.fillMaxWidth().weight(1f)) {
            items(items) { i -> Text(i, style = MaterialTheme.typography.bodySmall) }
        }
        HorizontalDivider()
        Text("${props["$MOD.count"] ?: ""}: ${props["$MOD.itemCount"] ?: ""}", style = MaterialTheme.typography.bodyMedium)
        Text("${props["$MOD.subtotal"] ?: ""}: ${props["$MOD.subtotal"] ?: ""}", style = MaterialTheme.typography.bodyMedium)
        Text("${props["$MOD.tax"] ?: ""}: ${props["$MOD.tax"] ?: ""}", style = MaterialTheme.typography.bodyMedium)
        Text("${props["$MOD.total"] ?: ""}: ${props["$MOD.total"] ?: ""}", style = MaterialTheme.typography.bodyMedium)
    }
}
