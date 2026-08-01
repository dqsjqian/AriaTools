#pragma once
//
// CalendarModel — 日历模块的模块级共享业务状态中心（非单个 View 的 VM）。
//
// 职责：
//   * 持有订阅列表、当前年月、当月 42 格 DayCell（含前后月补齐、今天高亮）、事件；
//   * 经 CalendarService 抓取/解析 .ics 并按日期归类到格子；
//   * 上/下月、回今天、增删订阅、刷新。
//
// 日期换算（当月首日星期、补齐、今天判定、事件归类）全在此完成，View 只渲染。
//
#include "models/CalendarTypes.h"
#include "services/CalendarService.h"

#include "aria/aria.hpp"

#include <memory>
#include <string>

namespace wb::calendar {

class CalendarModel {
public:
    explicit CalendarModel(std::shared_ptr<CalendarService> service);

    // 响应式状态：VM 直接绑定。
    aria::ObservableList<Subscription> subscriptions;
    aria::ObservableList<DayCell>      days;      ///< 固定 42 格
    aria::Property<int>                year{0};
    aria::Property<int>                month{0};   ///< 1..12
    aria::Property<std::string>        status{""};
    aria::Property<std::string>        lastError{""};

    // 初始化：定位到今天所在月，加载订阅与缓存事件并排布。
    void initialize();

    // 导航
    void prev_month();
    void next_month();
    void go_today();

    // 订阅
    bool add_subscription(const std::string& url, const std::string& name);
    bool remove_subscription(const SubId& id);

    // 抓取所有订阅（联网）→ 汇总事件 → 重排当月。
    bool refresh_all();

private:
    void rebuild_days_();                 ///< 依据 year/month + events_ 生成 42 格
    void set_error_(CalendarError e, std::string msg);
    static int weekday_of_(int y, int m, int d);   ///< 0=周一 .. 6=周日
    static int days_in_month_(int y, int m);

    std::shared_ptr<CalendarService> service_;
    EventList events_;                    ///< 所有订阅汇总的事件（全量，rebuild 时按当月过滤）
};

}  // namespace wb::calendar
