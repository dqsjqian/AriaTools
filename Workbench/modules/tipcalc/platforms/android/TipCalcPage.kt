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

private const val MOD = "tipcalc"

/**
 * TipCalcPage — Android (Compose) view for the "tipcalc" module.
 * Bill/tip%/people inputs (numeric strings) + round up button + results.
 * Numeric inputs are written as strings; the VM-side converter parses them.
 */
@Composable
fun TipCalcPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "tipcalc", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        HorizontalDivider()
        OutlinedTextField(value = props["$MOD.bill"] ?: "", onValueChange = { vm.setText(MOD, "bill", it) }, label = { Text(props["$MOD.bill_label"] ?: "Bill") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.tipPercent"] ?: "", onValueChange = { vm.setText(MOD, "tipPercent", it) }, label = { Text(props["$MOD.tip_label"] ?: "Tip %") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        OutlinedTextField(value = props["$MOD.people"] ?: "", onValueChange = { vm.setText(MOD, "people", it) }, label = { Text(props["$MOD.people_label"] ?: "People") }, singleLine = true, modifier = Modifier.fillMaxWidth())
        Button(onClick = { vm.execute(MOD, "roundUp") }, modifier = Modifier.fillMaxWidth()) { Text(props["$MOD.round_up"] ?: "Round up") }
        HorizontalDivider()
        Text("${props["$MOD.tip_amount"] ?: ""}: ${props["$MOD.tipAmount"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("${props["$MOD.total"] ?: ""}: ${props["$MOD.total"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("${props["$MOD.per_person"] ?: ""}: ${props["$MOD.perPerson"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
