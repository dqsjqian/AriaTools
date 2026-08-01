#pragma once
//
// SyncVm — 数据同步中心：既管远端仓库配置（数据目录/远程/分支/账号/Token/自动同步/
// 加密范围），也提供同步/拉取/推送动作与状态日志。配置与动作同属「同步」功能，
// 故合于一处。文案经 i18n 门面。
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

    // ── 远端仓库配置（可编辑）──
    aria::Property<std::string> dataDir;
    aria::Property<std::string> remoteUrl;
    aria::Property<std::string> branch;
    aria::Property<std::string> username;
    aria::Property<std::string> token;
    aria::Property<bool>        autoSync;
    aria::Property<int>         encryptScope;

    // ── 运行状态 ──
    aria::Property<std::string> status;
    aria::Property<std::string> log;

    // ── 文案（随语言更新）──
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

    // ── 命令 ──
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
