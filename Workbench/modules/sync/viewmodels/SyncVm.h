#pragma once
//
// SyncVm — 数据同步（"同步"按钮 + 状态）。文案经 i18n 派发。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"
#include "infra/sync/ISyncService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/secret/ISecretStore.h"

namespace wb::sync {

class SyncVm final : public wb::core::LocalizedVm {
public:
    SyncVm(wb::services::II18nService& i18n,
           wb::services::ISyncService& sync,
           wb::services::ISettingsService& settings,
           wb::services::ISecretStore& secrets);

    aria::Property<std::string> status;
    aria::Property<std::string> log;

    // 文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> syncLabel;
    aria::Property<std::string> pullLabel;
    aria::Property<std::string> pushLabel;

    aria::Command<> syncNow;
    aria::Command<> pushOnly;
    aria::Command<> pullOnly;

    void on_activate() override;
    void on_deactivate() override;

private:
    wb::services::ISyncService& sync_;
    wb::services::ISettingsService& settings_;
    wb::services::ISecretStore& secrets_;

    void append_log_(const std::string& line);
    std::string token_() const;
};

}  // namespace wb::sync
