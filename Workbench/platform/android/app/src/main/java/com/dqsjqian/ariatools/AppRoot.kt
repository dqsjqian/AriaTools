package com.dqsjqian.ariatools

import android.content.Intent
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.FilterChip
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.ui.ComposeViewFactory

/**
 * AppRoot — Compose shell. Bottom navigation over the C++ module list; the
 * current module's page is resolved through ComposeViewFactory (each module
 * self-registers from its platforms/android sources, symmetric with Qt/iOS).
 */
@Composable
fun AppRoot(viewModel: AppViewModel) {
    val modules by viewModel.modules.collectAsState()
    val current by viewModel.current.collectAsState()
    val context = LocalContext.current

    Scaffold(
        bottomBar = {
            NavigationBar {
                Row(
                    modifier = Modifier.fillMaxWidth()
                        .horizontalScroll(rememberScrollState())
                        .padding(horizontal = 8.dp),
                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    modules.forEach { m ->
                        FilterChip(
                            selected = current == m.id,
                            onClick = { viewModel.select(m.id) },
                            label = { Text(m.title, maxLines = 1) },
                        )
                    }
                }
            }
        }
    ) { padding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(padding)
                .padding(16.dp)
        ) {
            Button(onClick = {
                context.startActivity(Intent(context, ViewBindingLabActivity::class.java))
            }) {
                Text(context.getString(R.string.jni_lab_entry))
            }

            val page = current?.let { ComposeViewFactory.build(it, viewModel) }
            if (page != null) {
                page()
            } else {
                Text(
                    "module page not registered: $current",
                    style = MaterialTheme.typography.bodyMedium,
                )
            }
        }
    }
}
