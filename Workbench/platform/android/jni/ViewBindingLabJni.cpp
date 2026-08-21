#include "aria/adapters/jni/JniAdapter.hpp"
#include "aria/binding/binding_engine.hpp"
#include "aria/command.hpp"
#include "aria/property.hpp"

#include <jni.h>

#include <memory>
#include <string>

namespace {

class ViewBindingLab {
public:
    ViewBindingLab(JNIEnv* env, jobject edit_text, jobject button, jobject label)
        : adapter_(std::make_shared<aria::adapters::jni::JniAdapter>(env)),
          engine_(adapter_),
          edit_(env, edit_text),
          button_(env, button),
          label_(env, label),
          greet_([this] { greeting_ = "Hello, " + name_.get() + "!"; }) {
        engine_.bind_text(name_, edit_);
        engine_.bind_command(greet_, button_);
        engine_.bind_text_oneway(greeting_, label_);
    }

    void text_changed(JNIEnv* env, jstring text) {
        const char* chars = text ? env->GetStringUTFChars(text, nullptr) : nullptr;
        const std::string value = chars ? chars : "";
        if (chars) env->ReleaseStringUTFChars(text, chars);
        adapter_->notify_text_changed(edit_, value);
    }

    void click() { adapter_->notify_click(button_); }

private:
    std::shared_ptr<aria::adapters::jni::JniAdapter> adapter_;
    aria::binding::BindingEngine engine_;
    aria::adapters::jni::JniView edit_;
    aria::adapters::jni::JniView button_;
    aria::adapters::jni::JniView label_;
    aria::Property<std::string> name_{"World"};
    aria::Property<std::string> greeting_{"Edit the name, then tap the button."};
    aria::Command<> greet_;
};

ViewBindingLab* from_handle(jlong handle) {
    return reinterpret_cast<ViewBindingLab*>(handle);
}

}  // namespace

extern "C" JNIEXPORT jlong JNICALL
Java_com_dqsjqian_ariatools_ViewBindingLabActivity_nativeCreate(
    JNIEnv* env, jobject, jobject edit_text, jobject button, jobject label) {
    return reinterpret_cast<jlong>(
        new ViewBindingLab(env, edit_text, button, label));
}

extern "C" JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_ViewBindingLabActivity_nativeTextChanged(
    JNIEnv* env, jobject, jlong handle, jstring text) {
    if (auto* lab = from_handle(handle)) lab->text_changed(env, text);
}

extern "C" JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_ViewBindingLabActivity_nativeClick(
    JNIEnv*, jobject, jlong handle) {
    if (auto* lab = from_handle(handle)) lab->click();
}

extern "C" JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_ViewBindingLabActivity_nativeDestroy(
    JNIEnv*, jobject, jlong handle) {
    delete from_handle(handle);
}
