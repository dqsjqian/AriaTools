// ────────────────────────────────────────────────────────────────────────────
//  ToolsJniBinding.cpp — see ToolsJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/ToolsJniBinding.h"

namespace wb::tools {

void subscribe_tools(aria::runtime::EventBus& bus, ToolsVm& vm,
                     std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "tools", "title", vm.title);
    bind_str(subs, "tools", "base64Input",  vm.base64Input);
    bind_str(subs, "tools", "base64Output", vm.base64Output);
    bind_str(subs, "tools", "randomOutput", vm.randomOutput);
    bind_str(subs, "tools", "jsonInput",    vm.jsonInput);
    bind_str(subs, "tools", "jsonOutput",   vm.jsonOutput);
    // Labels (i18n).
    bind_str(subs, "tools", "base64_group", vm.base64Group);
    bind_str(subs, "tools", "random_group", vm.randomGroup);
    bind_str(subs, "tools", "json_group",   vm.jsonGroup);
    bind_str(subs, "tools", "input",  vm.inputLabel);
    bind_str(subs, "tools", "output", vm.outputLabel);
    bind_str(subs, "tools", "encode", vm.encodeLabel);
    bind_str(subs, "tools", "decode", vm.decodeLabel);
    bind_str(subs, "tools", "length", vm.lengthLabel);
    bind_str(subs, "tools", "generate", vm.generateLabel);
    bind_str(subs, "tools", "format", vm.formatLabel);
    bind_str(subs, "tools", "minify", vm.minifyLabel);
}

void set_tools_text(ToolsVm& vm, const std::string& propName,
                    const std::string& value) {
    if (propName == "base64Input")      vm.base64Input.set(value);
    else if (propName == "jsonInput")    vm.jsonInput.set(value);
}

void exec_tools_command(ToolsVm& vm, const std::string& cmdName) {
    if (cmdName == "encodeBase64") vm.base64Encode.execute();
    else if (cmdName == "decodeBase64") vm.base64Decode.execute();
    else if (cmdName == "generateRandom") vm.genRandom.execute();
    else if (cmdName == "formatJson") vm.jsonFormat.execute();
    else if (cmdName == "minifyJson") vm.jsonMinify.execute();
}

void register_tools_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_tools(bus, static_cast<ToolsVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_tools_text(static_cast<ToolsVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_tools_command(static_cast<ToolsVm&>(vm), cmdName);
    };
    table.emplace("tools", b);
}

}  // namespace wb::tools
