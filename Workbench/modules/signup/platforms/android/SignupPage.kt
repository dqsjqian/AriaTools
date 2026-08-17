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
 * SignupPage — Android (Compose) view for the "signup" module.
 * Renders the C++ VM's headline properties pushed over JNI.
 */
@Composable
fun SignupPage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp)
    ) {
        Text(props["signup.title"] ?: "signup", style = MaterialTheme.typography.headlineSmall)
        Spacer(Modifier.height(8.dp))
        Text(props["signup.desc"] ?: props["signup.hint"] ?: "", style = MaterialTheme.typography.bodyMedium)
        Spacer(Modifier.height(16.dp))
        Text("title: ${props["signup.title"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("desc: ${props["signup.desc"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("submittedSummary: ${props["signup.submittedSummary"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("username_error: ${props["signup.username_error"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("email_error: ${props["signup.email_error"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("password_error: ${props["signup.password_error"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
        Text("confirm_error: ${props["signup.confirm_error"] ?: ""}", style = MaterialTheme.typography.bodyLarge)
    }
}
