package com.dqsjqian.ariatools.pages

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.dqsjqian.ariatools.AppViewModel

private const val MOD = "theme"

/**
 * ThemePage — Android (Compose) view for the "theme" module.
 * Three theme picker buttons + current display name + sample card that
 * changes background/foreground based on the selected theme.
 */
@Composable
fun ThemePage(vm: AppViewModel) {
    val props by vm.props.collectAsState()
    Column(
        modifier = Modifier.fillMaxSize().padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(props["$MOD.title"] ?: "theme", style = MaterialTheme.typography.headlineSmall)
        Text(props["$MOD.desc"] ?: "", style = MaterialTheme.typography.bodySmall)
        Text(props["$MOD.currentDisplayName"] ?: "", style = MaterialTheme.typography.titleMedium)
        HorizontalDivider()
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = { vm.execute(MOD, "pickLight") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_light"] ?: "Light") }
            Button(onClick = { vm.execute(MOD, "pickDark") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_dark"] ?: "Dark") }
            Button(onClick = { vm.execute(MOD, "pickSolarized") }, modifier = Modifier.weight(1f)) { Text(props["$MOD.theme_solarized"] ?: "Solarized") }
        }
        // Sample card — background color reflects the current theme.
        val bg = when (props["$MOD.currentId"] ?: "light") {
            "dark" -> Color(0xFF263238); "solarized" -> Color(0xFFFDF6E3); else -> Color.White
        }
        val fg = when (props["$MOD.currentId"] ?: "light") {
            "dark" -> Color(0xFFECEFF1); "solarized" -> Color(0xFF586E75); else -> Color(0xFF212121)
        }
        Box(modifier = Modifier.fillMaxWidth().size(120.dp).background(bg)) {
            Text(props["$MOD.card_title"] ?: "Card", color = fg, style = MaterialTheme.typography.titleMedium, modifier = Modifier.padding(16.dp))
        }
    }
}
