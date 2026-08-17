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
 * CartPage — Android (Compose) view for the "cart" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun CartPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["cart.title"] ?: "cart", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["cart.desc"] ?: props["cart.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["cart.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("desc: ${props["cart.desc"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("itemCount: ${props["cart.itemCount"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("subtotal: ${props["cart.subtotal"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("tax: ${props["cart.tax"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("total: ${props["cart.total"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
