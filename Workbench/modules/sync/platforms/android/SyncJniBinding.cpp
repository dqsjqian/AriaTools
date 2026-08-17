// ────────────────────────────────────────────────────────────────────────────
//  SyncJniBinding.cpp — see SyncJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/SyncJniBinding.h"

namespace wb::sync {

void subscribe_sync(aria::runtime::EventBus& bus, SyncVm& vm,
                    std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;

    bind_str(subs, "sync", "title", vm.title);
    bind_str(subs, "sync", "hint",  vm.hint);
    bind_str(subs, "sync", "status", vm.status);
    // Config fields (two-way editable).
    bind_str(subs, "sync", "dataDir",   vm.dataDir);
    bind_str(subs, "sync", "remote",     vm.remoteUrl);
    bind_str(subs, "sync", "branch",     vm.branch);
    bind_str(subs, "sync", "username",   vm.username);
    bind_str(subs, "sync", "token",      vm.token);
    // Labels (i18n).
    bind_str(subs, "sync", "data_dir",    vm.dataDirLabel);
    bind_str(subs, "sync", "remote_label",vm.remoteLabel);
    bind_str(subs, "sync", "branch_label",vm.branchLabel);
    bind_str(subs, "sync", "username",    vm.usernameLabel);
    bind_str(subs, "sync", "token",       vm.tokenLabel);
    bind_str(subs, "sync", "save_config", vm.saveLabel);
    bind_str(subs, "sync", "sync_now",    vm.syncLabel);
    bind_str(subs, "sync", "pull",        vm.pullLabel);
    bind_str(subs, "sync", "push",        vm.pushLabel);
}

void set_sync_text(SyncVm& vm, const std::string& propName,
                   const std::string& value) {
    if (propName == "dataDir")      vm.dataDir.set(value);
    else if (propName == "remote")  vm.remoteUrl.set(value);
    else if (propName == "branch")  vm.branch.set(value);
    else if (propName == "username")vm.username.set(value);
    else if (propName == "token")   vm.token.set(value);
}

void exec_sync_command(SyncVm& vm, const std::string& cmdName) {
    if (cmdName == "saveConfig") vm.saveConfig.execute();
    else if (cmdName == "syncNow") vm.syncNow.execute();
    else if (cmdName == "pull")    vm.pullOnly.execute();
    else if (cmdName == "push")    vm.pushOnly.execute();
}

void register_sync_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_sync(bus, static_cast<SyncVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_sync_text(static_cast<SyncVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_sync_command(static_cast<SyncVm&>(vm), cmdName);
    };
    table.emplace("sync", b);
}

}  // namespace wb::sync
