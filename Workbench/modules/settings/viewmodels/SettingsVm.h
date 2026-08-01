#pragma once
//
// SettingsVm — 应用级偏好设置（当前：界面语言；后续可加主题色等）。
// 同步/远端仓库配置已移至 Sync 模块（配置与同步动作同属一功能）。
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"

namespace wb::settings {

class SettingsVm final : public wb::core::BaseVm {
public:
    SettingsVm();

    aria::Property<std::string> language;   ///< 当前语言代码

    // 界面文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> languageLabel;

    /// 切换语言（View 传入语言代码，如 "zh-CN" / "en"）。
    aria::Command<std::string> switchLanguage;

    void on_activate() override;
    void on_deactivate() override;
};

}  // namespace wb::settings
