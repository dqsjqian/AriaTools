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

private const val MOD = "notes"

/**
 * NotesPage — Android (Compose) view for the "notes" module.
 * Editable title + body (two-way bound), add/save/delete buttons,
 * and a note list rendered from VM state.
 *
 * Decomposed into sub-composables:
 *   NoteActionBar — add/save/delete buttons
 *   NoteEditor    — title + body fields (two-way bound)
 *   NoteList      — list of saved notes
 */
@Composable
fun NotesPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Text(props["$MOD.title"] ?: "notes", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.hint"] ?: "", style = MaterialTheme.typography.bodySmall)
        Text(props["$MOD.status"] ?: "", style = MaterialTheme.typography.bodyMedium)

        NoteActionBar(vm, props)
        NoteEditor(vm, props)
        HorizontalDivider()
        NoteList(props)
    }
}

// ─── Sub-composables ──────────────────────────────────────────────────────

@Composable
private fun NoteActionBar(vm: AppViewModel, props: Map<String, String>) {
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Button(onClick = { vm.execute(MOD, "addNote") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.add"] ?: "Add") }
        Button(onClick = { vm.execute(MOD, "saveNote") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.save"] ?: "Save") }
        Button(onClick = { vm.execute(MOD, "deleteSelected") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.delete"] ?: "Delete") }
    }
}

@Composable
private fun NoteEditor(vm: AppViewModel, props: Map<String, String>) {
    OutlinedTextField(
        value = props["$MOD.editTitle"] ?: "",
        onValueChange = { vm.setText(MOD, "editTitle", it) },
        label = { Text(props["$MOD.title_placeholder"] ?: "Title") },
        singleLine = true,
        modifier = Modifier.fillMaxWidth(),
    )
    OutlinedTextField(
        value = props["$MOD.editBody"] ?: "",
        onValueChange = { vm.setText(MOD, "editBody", it) },
        label = { Text(props["$MOD.body_placeholder"] ?: "Body") },
        modifier = Modifier.fillMaxWidth(),
    )
}

@Composable
private fun NoteList(props: Map<String, String>) {
    val list = (props["$MOD.noteList"] ?: "").split('\n').filter { it.isNotBlank() }
    LazyColumn(modifier = Modifier.fillMaxWidth()) {
        items(list) { t -> Text(t, style = MaterialTheme.typography.bodySmall) }
    }
}
