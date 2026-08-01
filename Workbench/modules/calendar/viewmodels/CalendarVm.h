#pragma once
//
// CalendarVm — 日历 ViewModel。只维护页面响应式状态与 Command，
// 月历排布/订阅抓取全委托 CalendarModel；View 只绑定 VM。
// 文案经全局 wb::i18n 门面绑定（无需继承 i18n 基类、无需注入 II18nService）。
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "models/CalendarModel.h"

#include <memory>

namespace wb::calendar {

class CalendarVm final : public wb::core::BaseVm {
public:
    explicit CalendarVm(std::shared_ptr<CalendarModel> model);

    // 月历数据（转发自 Model，供 View 绑定）。
    aria::ObservableList<DayCell>&      days;
    aria::ObservableList<Subscription>& subscriptions;
    aria::Property<std::string>         monthTitle;   ///< "2026-07"（动态）
    aria::Property<std::string>&        status;

    // 订阅输入
    aria::Property<std::string> subscribeUrl;

    // 命令
    aria::Command<>            prevMonth;
    aria::Command<>            nextMonth;
    aria::Command<>            today;
    aria::Command<>            refresh;
    aria::Command<>            addSubscription;
    aria::Command<std::string> removeSubscription;  ///< 参数：订阅 id

    // 文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> urlPlaceholder;
    aria::Property<std::string> subscribeLabel;
    aria::Property<std::string> prevLabel;
    aria::Property<std::string> nextLabel;
    aria::Property<std::string> todayLabel;
    aria::Property<std::string> refreshLabel;
    // 周表头（周一..周日）
    aria::Property<std::string> wd1, wd2, wd3, wd4, wd5, wd6, wd7;

    void on_activate() override;
    void on_deactivate() override;

private:
    void refresh_month_title_();

    std::shared_ptr<CalendarModel> model_;
};

}  // namespace wb::calendar
