#pragma once
//
// CalendarVm — 日历（第三方 .ics 订阅）。文案经 i18n 派发。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"

namespace wb::calendar {

class CalendarVm final : public wb::core::LocalizedVm {
public:
    explicit CalendarVm(wb::services::II18nService& i18n);

    aria::Property<std::string> subscribeUrl;   ///< 输入（用户驱动）

    // 文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> urlPlaceholder;
    aria::Property<std::string> subscribeLabel;
    aria::Property<std::string> status;   ///< 动态

    aria::Command<> addSubscription;

    void on_activate() override;
    void on_deactivate() override;

private:
    int subCount_ = 0;
    void refresh_status_();
};

}  // namespace wb::calendar
