#pragma once
//
// SyncVm — data sync center: manages remote repository configuration (data directory/remote/branch/account/Token/auto-sync/
// encryption scope), and provides sync/pull/push actions plus a status log. Configuration and actions both belong to the "sync" feature,
// so they are kept together. Text goes through the i18n facade.
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "infra/sync/ISyncService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/secret/ISecretStore.h"

namespace wb::sync {

class SyncVm final : public wb::core::BaseVm {
public:
    SyncVm(wb::services::ISyncService& sync,
           wb::services::ISettingsService& settings,
           wb::services::ISecretStore& secrets);

    // ── Remote repository configuration (editable) ──
    aria::Property<std::string> dataDir;
    aria::Property<std::string> remoteUrl;
    aria::Property<std::string> branch;
    aria::Property<std::string> username;
    aria::Property<std::string> token;
    aria::Property<bool>        autoSync;
    aria::Property<int>         encryptScope;

    // ── Runtime status ──
    aria::Property<std::string> status;
    aria::Property<std::string> log;

    // ── Text (updates on language change) ──
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> dataDirLabel;
    aria::Property<std::string> remoteLabel;
    aria::Property<std::string> branchLabel;
    aria::Property<std::string> usernameLabel;
    aria::Property<std::string> tokenLabel;
    aria::Property<std::string> autoSyncLabel;
    aria::Property<std::string> saveLabel;
    aria::Property<std::string> syncLabel;
    aria::Property<std::string> pullLabel;
    aria::Property<std::string> pushLabel;

    // ── Commands ──
    aria::Command<> saveConfig;
    aria::Command<> syncNow;
    aria::Command<> pushOnly;
    aria::Command<> pullOnly;

    void on_activate() override;
    void on_deactivate() override;

private:
    wb::services::ISyncService& sync_;
    wb::services::ISettingsService& settings_;
    wb::services::ISecretStore& secrets_;

    void load_config_();
    void save_config_();
    void append_log_(const std::string& line);
    std::string token_() const;
};

}  // namespace wb::sync
