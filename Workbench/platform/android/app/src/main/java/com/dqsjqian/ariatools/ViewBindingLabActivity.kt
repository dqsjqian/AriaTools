package com.dqsjqian.ariatools

import android.app.Activity
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.Gravity
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView

/**
 * Native Android View lab for Aria's typed JniAdapter path.
 *
 * The main application intentionally uses Compose + StateFlow. This small
 * Activity demonstrates the other supported integration shape: real Android
 * View objects wrapped by JniView and bound through BindingEngine.
 */
class ViewBindingLabActivity : Activity() {
    private companion object {
        init {
            // This Activity is a standalone entry point: Android may recreate
            // it after process death without MainActivity running first, so it
            // cannot rely on MainActivity's loadLibrary call. Repeat loads of
            // the same library are no-ops.
            System.loadLibrary("aria_jni")
        }
    }

    private var nativeHandle: Long = 0

    private external fun nativeCreate(
        editText: EditText,
        button: Button,
        label: TextView,
    ): Long
    private external fun nativeTextChanged(handle: Long, value: String)
    private external fun nativeClick(handle: Long)
    private external fun nativeDestroy(handle: Long)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val density = resources.displayMetrics.density
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER_HORIZONTAL
            setPadding((24 * density).toInt(), (48 * density).toInt(),
                (24 * density).toInt(), (24 * density).toInt())
        }
        val title = TextView(this).apply {
            text = getString(R.string.jni_lab_title)
            textSize = 24f
        }
        val explanation = TextView(this).apply {
            text = getString(R.string.jni_lab_description)
            textSize = 14f
        }
        val edit = EditText(this).apply { hint = getString(R.string.jni_lab_name_hint) }
        val button = Button(this).apply { text = getString(R.string.jni_lab_greet) }
        val result = TextView(this).apply { textSize = 20f }

        for (view in listOf(title, explanation, edit, button, result)) {
            root.addView(view, ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.WRAP_CONTENT,
            ))
        }
        setContentView(root)

        nativeHandle = nativeCreate(edit, button, result)
        edit.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) = Unit
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                if (nativeHandle != 0L) nativeTextChanged(nativeHandle, s?.toString().orEmpty())
            }
            override fun afterTextChanged(s: Editable?) = Unit
        })
        button.setOnClickListener {
            if (nativeHandle != 0L) nativeClick(nativeHandle)
        }
    }

    override fun onDestroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0
        }
        super.onDestroy()
    }
}
