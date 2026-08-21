package com.dqsjqian.ariatools

import android.content.Intent
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
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
                modules.forEach { m ->
                    NavigationBarItem(
                        selected = current == m.id,
                        onClick = { viewModel.select(m.id) },
                        icon = {},
                        label = { Text(m.title) },
                    )
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
