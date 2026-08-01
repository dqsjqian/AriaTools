#pragma once
//
// CalendarService — 日历模块私有业务服务（纯 C++，跨平台）。
//
// 职责：
//   * ICS 文本解析：VEVENT / SUMMARY / DTSTART / DTEND（RFC5545 折行、
//     DATE 与 DATE-TIME、UTC 'Z'），MVP 不展开 RRULE。
//   * 订阅持久化：一订阅一文件 calendar/subs/<id>.ini，抓取缓存
//     calendar/cache/<id>.ics（均经 IStorageService）。
//   * refresh：经 IHttpClient 抓取远程 .ics → 写缓存 → 解析。
//
// 网络与存储都经注入接口，本服务与平台无关；VM/Model 只依赖本服务。
//
#include "models/CalendarTypes.h"

#include "infra/storage/IStorageService.h"
#include "infra/http/IHttpClient.h"

#include <string>

namespace wb::calendar {

class CalendarService {
public:
    CalendarService(wb::services::IStorageService& storage,
                    wb::services::IHttpClient& http)
        : storage_(storage), http_(http) {}

    // 订阅持久化
    [[nodiscard]] CalendarResult<SubscriptionList> load_subscriptions();
    [[nodiscard]] CalendarResult<Subscription> add_subscription(const std::string& url,
                                                                const std::string& name);
    [[nodiscard]] CalendarResult<void> remove_subscription(const SubId& id);

    // 抓取 + 解析：返回该订阅的事件列表，并更新缓存与 lastFetched。
    [[nodiscard]] CalendarResult<EventList> refresh(const Subscription& sub);

    // 读缓存（离线）：解析已缓存的 .ics，不联网。
    [[nodiscard]] CalendarResult<EventList> load_cached(const SubId& id);

    // 纯解析（供测试直接调用，不碰 IO）。
    [[nodiscard]] static CalendarResult<EventList> parse_ics(std::string_view text,
                                                             const SubId& source);

private:
    [[nodiscard]] CalendarResult<void> save_subscription_(const Subscription& sub);

    wb::services::IStorageService& storage_;
    wb::services::IHttpClient& http_;
};

}  // namespace wb::calendar
