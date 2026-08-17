package com.dqsjqian.ariatools

import androidx.lifecycle.ViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow

/** A navigation entry reported by the C++ core (id + localized title). */
data class ModuleInfo(val id: String, val title: String)

/**
 * AppViewModel — thin Kotlin shell holding the StateFlows that Compose
 * observes. All values originate from the C++ core via JniBridge.
 *
 * Per-module state is stored in a flat namespace keyed by "<module>.<prop>"
 * (e.g. "dashboard.welcome"). Pages read the value they need via
 * [prop]; the JniBridge pushes updates from the C++ VM layer.
 */
class AppViewModel : ViewModel() {

    private val _modules = MutableStateFlow<List<ModuleInfo>>(emptyList())
    val modules: StateFlow<List<ModuleInfo>> = _modules.asStateFlow()

    private val _current = MutableStateFlow<String?>(null)
    val current: StateFlow<String?> = _current.asStateFlow()

    // Flat property store: "<moduleId>.<propName>" → value (string form).
    private val _props = MutableStateFlow<Map<String, String>>(emptyMap())
    val props: StateFlow<Map<String, String>> = _props.asStateFlow()

    // ── Lifecycle ─────────────────────────────────────────────────────────

    /** Create the C++ shell; the module list arrives via onModulesChanged. */
    fun initShell(i18nDir: String) {
        JniBridge.attachViewModel(this, i18nDir)
    }

    override fun onCleared() {
        JniBridge.detachViewModel()
        super.onCleared()
    }

    // ── JNI callbacks (main thread) ───────────────────────────────────────

    fun onModulesChanged(ids: Array<String>, titles: Array<String>) {
        val list = ids.indices.map { ModuleInfo(ids[it], titles.getOrElse(it) { ids[it] }) }
        _modules.value = list
        if (_current.value == null && list.isNotEmpty()) {
            select(list.first().id)
        }
    }

    fun onPropertyChanged(moduleId: String, propName: String, value: String) {
        val key = "$moduleId.$propName"
        _props.value = _props.value + (key to value)
    }

    /** Convenience accessor used by module Pages. */
    fun prop(moduleId: String, propName: String): String? =
        _props.value["$moduleId.$propName"]

    /** Convenience accessor returning the latest value or a default. */
    fun propOr(moduleId: String, propName: String, default: String): String =
        prop(moduleId, propName) ?: default

    // ── Navigation ────────────────────────────────────────────────────────

    fun select(id: String) {
        if (_current.value == id) return
        _current.value = id
        JniBridge.activateModule(id)
    }
}
