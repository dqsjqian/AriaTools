package com.dqsjqian.ariatools

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.ui.graphics.Color
import com.dqsjqian.ariatools.ui.registerAllModulePages
import java.io.File

/**
 * MainActivity — Workbench Android entry.
 *
 * 1. Copies the i18n resources bundled in assets/ to filesDir/i18n (AppCore
 *    reads strings from a plain directory).
 * 2. Creates the C++ shell (AppCore + module VMs) via JniBridge.
 * 3. Renders the Compose UI; module list + property values stream in over JNI.
 */
class MainActivity : ComponentActivity() {

    private val appViewModel: AppViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        System.loadLibrary("aria_jni")
        registerAllModulePages()

        val i18nDir = copyAssetsI18n()
        appViewModel.initShell(i18nDir)

        setContent {
            val dark = isSystemInDarkTheme()
            MaterialTheme(colorScheme = if (dark) DarkColors else LightColors) {
                AppRoot(appViewModel)
            }
        }
    }

    // ── assets/{mod}/i18n-*.xml → filesDir/i18n/{mod}/ ────────────────────
    // The Gradle build maps build/platforms/android/i18n into the APK's
    // assets root (assets/calendar/strings.xml, assets/dashboard/...), so we
    // copy every top-level asset directory (one per module) to filesDir/i18n.
    private fun copyAssetsI18n(): String {
        val dest = File(filesDir, "i18n")

        fun copyDir(from: String, to: File) {
            assets.list(from).orEmpty().forEach { name ->
                val childFrom = if (from.isEmpty()) name else "$from/$name"
                val childTo = File(to, name)
                val isDir = assets.list(childFrom)?.isNotEmpty() == true
                if (isDir) {
                    childTo.mkdirs()
                    copyDir(childFrom, childTo)
                } else {
                    childTo.parentFile?.mkdirs()
                    assets.open(childFrom).use { input ->
                        childTo.outputStream().use { output -> input.copyTo(output) }
                    }
                }
            }
        }

        dest.mkdirs()
        copyDir("", dest)
        return dest.absolutePath
    }

    private val LightColors = lightColorScheme(
        primary = Color(0xFF1A237E),
        background = Color(0xFFFAFAFA),
        surface = Color(0xFFFFFFFF),
    )

    private val DarkColors = darkColorScheme(
        primary = Color(0xFF9FA8DA),
        background = Color(0xFF121212),
        surface = Color(0xFF1E1E1E),
    )
}
