package com.dqsjqian.ariatools

import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow

/**
 * JniBridge — JNI side-channel bridge between the C++ Workbench core and Kotlin.
 *
 * Architecture (same as Aria demo5):
 *   C++ AppCore / module VMs (aria::Property) → on_changed →
 *   JNI callback → JniBridge.onPropertyChanged → Kotlin StateFlow → Compose.
 *
 * The C++ side owns ALL business logic. This bridge only:
 *   1. Creates/destroys the C++ AndroidShell (which owns the AppCore)
 *   2. Receives module list + property change notifications from C++
 *   3. Forwards them to the Kotlin AppViewModel's StateFlows
 *   4. Delegates navigation (module activation) to C++
 */
object JniBridge {
    private const val TAG = "WbJniBridge"

    private var viewModel: AppViewModel? = null

    private val mainHandler = Handler(Looper.getMainLooper())

    init {
        System.loadLibrary("aria_jni")
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────

    fun attachViewModel(vm: AppViewModel, i18nDir: String) {
        viewModel = vm
        nativeCreateShell(i18nDir)
    }

    fun detachViewModel() {
        nativeDestroyShell()
        viewModel = null
    }

    // ── JNI callbacks (called from C++ on the main thread) ────────────────

    /** Module list reported by C++ after shell creation. */
    @JvmStatic
    fun onModulesChanged(ids: Array<String>, titles: Array<String>) {
        Log.d(TAG, "modules: ${ids.size}")
        viewModel?.onModulesChanged(ids, titles)
    }

    /** A C++ ViewModel property changed. */
    @JvmStatic
    fun onPropertyChanged(moduleId: String, propName: String, newValue: String?) {
        Log.d(TAG, "property: $moduleId.$propName = $newValue")
        viewModel?.onPropertyChanged(moduleId, propName, newValue ?: "")
    }

    // ── Navigation delegates ──────────────────────────────────────────────

    fun activateModule(id: String) = nativeActivateModule(id)

    // ── JNI native method declarations ────────────────────────────────────

    @JvmStatic external fun nativeCreateShell(i18nDir: String)
    @JvmStatic external fun nativeDestroyShell()
    @JvmStatic external fun nativeActivateModule(id: String)
}
