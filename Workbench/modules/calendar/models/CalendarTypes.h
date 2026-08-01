#pragma once
//
// CalendarTypes — 日历模块的纯业务数据类型（跨平台，无 UI / 无平台依赖）。
//
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace wb::calendar {

using SubId = std::string;

/// 一个 .ics 远程订阅。
struct Subscription {
    SubId       id;              ///< 本地生成的稳定 id（= 文件名）
    std::string url;             ///< https://.../xxx.ics
    std::string name;            ///< 展示名（用户可填，默认取 URL host）
    std::int64_t lastFetched = 0; ///< 上次成功抓取时间（epoch 秒），0=从未
};

/// 一条日历事件（MVP：单次事件，不展开 RRULE 重复）。
struct CalendarEvent {
    std::string uid;             ///< VEVENT UID（去重用）
    std::string summary;         ///< 标题
    // 起止用「本地日期」三元组表示，便于按格归类；带时间的事件另存 HH:MM。
    int startYear = 0, startMonth = 0, startDay = 0;
    int endYear = 0, endMonth = 0, endDay = 0;
    bool allDay = true;          ///< DATE（全天）vs DATE-TIME
    std::string startTime;       ///< "HH:MM"（allDay 时为空）
    SubId       source;          ///< 来自哪个订阅
};

/// 月历中的一个格子（固定 6×7=42 格，含前后月补齐）。
struct DayCell {
    int year = 0, month = 0, day = 0;
    bool inCurrentMonth = false;
    bool isToday = false;
    std::string label;                    ///< 日数字文本
    std::vector<std::string> eventTitles; ///< 当天事件标题（供 View 直接渲染）
};

enum class CalendarError {
    None,
    FetchFailed,      ///< 网络抓取失败
    ParseFailed,      ///< ICS 解析失败
    InvalidData,      ///< 订阅数据非法（空 URL 等）
    ReadFailed,
    WriteFailed,
    RemoveFailed,
    NotFound,
};

template <typename T>
struct CalendarResult {
    T value{};
    CalendarError error = CalendarError::None;
    std::string message;

    [[nodiscard]] bool ok() const noexcept { return error == CalendarError::None; }

    static CalendarResult success(T value) {
        return {std::move(value), CalendarError::None, {}};
    }
    static CalendarResult failure(CalendarError error, std::string message) {
        return {{}, error, std::move(message)};
    }
};

template <>
struct CalendarResult<void> {
    CalendarError error = CalendarError::None;
    std::string message;

    [[nodiscard]] bool ok() const noexcept { return error == CalendarError::None; }

    static CalendarResult success() { return {}; }
    static CalendarResult failure(CalendarError error, std::string message) {
        return {error, std::move(message)};
    }
};

using SubscriptionList = std::vector<Subscription>;
using EventList = std::vector<CalendarEvent>;

}  // namespace wb::calendar
