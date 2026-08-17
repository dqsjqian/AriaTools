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
 * TipCalcPage — Android (Compose) view for the "tipcalc" module.
 * Renders the C++ VM's tip calculation properties pushed over JNI.
 */
@Composable
fun TipCalcPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["tipcalc.title"] ?: "TipCalc", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["tipcalc.desc"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("Bill: ${props["tipcalc.bill"] ?: "0"}", style = MaterialTheme.typography.bodyLarge)
        Text("Tip%: ${props["tipcalc.tipPercent"] ?: "0"}", style = MaterialTheme.typography.bodyLarge)
        Text("People: ${props["tipcalc.people"] ?: "0"}", style = MaterialTheme.typography.bodyLarge)
        Spacer(Modifier.height(16.dp))
        Text("Tip amount: ${props["tipcalc.tipAmount"] ?: "0"}", style = MaterialTheme.typography.titleMedium)
        Text("Total: ${props["tipcalc.total"] ?: "0"}", style = MaterialTheme.typography.titleMedium)
        Text("Per person: ${props["tipcalc.perPerson"] ?: "0"}", style = MaterialTheme.typography.titleMedium)
    }
}
