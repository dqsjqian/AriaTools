#pragma once
//
// DashboardVm — 首页概览。文案由 VM 经 i18n 派发（View 零字面量）。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"

namespace wb::dashboard {

class DashboardVm final : public wb::core::LocalizedVm {
public:
    explicit DashboardVm(wb::services::II18nService& i18n);

    aria::Property<std::string> welcome;   ///< 欢迎语（随语言更新）
    aria::Property<std::string> summary;   ///< 概览文本（随语言更新）

    void on_activate() override;
    void on_deactivate() override;
};

}  // namespace wb::dashboard
