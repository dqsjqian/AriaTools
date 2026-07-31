#pragma once
//
// SettingsVm — 设置（含同步设置 + 语言切换）。
// 同步仓库信息全部为运行时用户配置，绝不写死。文案经 i18n 派发。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"
#include "infra/settings/ISettingsService.h"
#include "infra/secret/ISecretStore.h"

namespace wb::settings {

class SettingsVm final : public wb::core::LocalizedVm {
public:
    SettingsVm(wb::services::II18nService& i18n,
               wb::services::ISettingsService& settings,
               wb::services::ISecretStore& secrets);

    // ── 同步设置（可编辑）──
    aria::Property<std::string> dataDir;
    aria::Property<std::string> remoteUrl;
    aria::Property<std::string> branch;
    aria::Property<std::string> username;
    aria::Property<std::string> token;
    aria::Property<bool>        autoSync;
    aria::Property<int>         encryptScope;
    aria::Property<std::string> language;   ///< 当前语言代码（双向）

    // ── 界面文案（随语言更新）──
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> dataDirLabel;
    aria::Property<std::string> remoteLabel;
    aria::Property<std::string> branchLabel;
    aria::Property<std::string> usernameLabel;
    aria::Property<std::string> tokenLabel;
    aria::Property<std::string> autoSyncLabel;
    aria::Property<std::string> languageLabel;
    aria::Property<std::string> saveLabel;
    aria::Property<std::string> status;

    aria::Command<> save;
    /// 切换语言（View 传入语言代码）。
    aria::Command<std::string> switchLanguage;

    void on_activate() override;
    void on_deactivate() override;

private:
    wb::services::ISettingsService& settings_;
    wb::services::ISecretStore& secrets_;

    void load_from_service_();
    void save_to_service_();
};

}  // namespace wb::settings
