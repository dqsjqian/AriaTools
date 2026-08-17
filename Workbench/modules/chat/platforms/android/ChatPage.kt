package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
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

private const val MOD = "chat"

/**
 * ChatPage — Android (Compose) view for the "chat" module.
 *
 * Real interaction: username + draft inputs (two-way bound to VM via
 * setText), send button (executes publisher.send), and a message list
 * rendered from the subscriber's ObservableList snapshot pushed over JNI.
 *
 * Symmetric with the Qt ChatView (QLineEdit × 2 + QPushButton + QListView)
 * and the iOS ChatView (UITextField × 2 + UIButton + UILabel list).
 */
@Composable
fun ChatPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "chat", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)

        HorizontalDivider()

        // Publisher: username + draft + send
        Text("Publisher", style = MaterialTheme.typography.titleSmall)
        OutlinedTextField(
            value = props["$MOD.user"] ?: "",
            onValueChange = { vm.setText(MOD, "user", it) },
            label = { Text(props["$MOD.user_label"] ?: "User") },
            singleLine = true,
            modifier = Modifier.fillMaxWidth(),
        )
        OutlinedTextField(
            value = props["$MOD.draft"] ?: "",
            onValueChange = { vm.setText(MOD, "draft", it) },
            label = { Text(props["$MOD.draft_label"] ?: "Message") },
            singleLine = true,
            modifier = Modifier.fillMaxWidth(),
        )
        Button(
            onClick = { vm.execute(MOD, "send") },
            modifier = Modifier.fillMaxWidth(),
        ) { Text(props["$MOD.send"] ?: "Send") }

        Spacer(Modifier.height(8.dp))
        HorizontalDivider()
        Text("Subscriber", style = MaterialTheme.typography.titleSmall)

        // Message list — pushed as a newline-joined string from C++ (each
        // line is "<user>: <text>"). A LazyColumn renders each line.
        val messages = (props["$MOD.messages"] ?: "").split('\n').filter { it.isNotBlank() }
        LazyColumn(
            modifier = Modifier.fillMaxWidth().weight(1f),
            verticalArrangement = Arrangement.spacedBy(4.dp),
        ) {
            items(messages) { line ->
                Text(line, style = MaterialTheme.typography.bodyMedium)
            }
        }
    }
}
