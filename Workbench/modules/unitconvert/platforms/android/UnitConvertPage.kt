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

private const val MOD = "unitconvert"

/**
 * UnitConvertPage — Android (Compose) view for the "unitconvert" module.
 * Category picker (3 buttons) + value input + converted result.
 */
@Composable
fun UnitConvertPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "unitconvert", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "selectTemperature") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.cat_temperature"] ?: "Temp") }
            Button(onClick = { vm.execute(MOD, "selectLength") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.cat_length"] ?: "Length") }
            Button(onClick = { vm.execute(MOD, "selectWeight") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.cat_weight"] ?: "Weight") }
        }
        OutlinedTextField(value = props["$MOD.value"] ?: "", onValueChange = { vm.setText(MOD, "value", it) }, label = { Text(props["$MOD.input"] ?: "Input") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        HorizontalDivider()
        Text("= ${props["$MOD.converted"] ?: ""}", style = MaterialTheme.typography.headlineSmall)
    }
}
