#pragma once
//
// CalendarTypes — Pure business data types of the calendar module (cross-platform, no UI / no platform dependencies).
//
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace wb::calendar {

using SubId = std::string;

/// A remote .ics subscription.
struct Subscription {
    SubId       id;              ///< Locally generated stable id (= file name)
    std::string url;             ///< https://.../xxx.ics
    std::string name;            ///< Display name (user may set; defaults to URL host)
    std::int64_t lastFetched = 0; ///< Last successful fetch time (epoch seconds); 0 = never
};

/// A calendar event (MVP: single occurrence; RRULE recurrence not expanded).
struct CalendarEvent {
    std::string uid;             ///< VEVENT UID (used for dedup)
    std::string summary;         ///< Title/summary
    // Start/end are represented as (year, month, day) triples of local dates for easy per-cell classification; timed events also store HH:MM.
    int startYear = 0, startMonth = 0, startDay = 0;
    int endYear = 0, endMonth = 0, endDay = 0;
    bool allDay = true;          ///< DATE (all-day) vs DATE-TIME
    std::string startTime;       ///< "HH:MM" (empty when allDay)
    SubId       source;          ///< Which subscription this came from
};

/// One cell of the month grid (fixed 6x7 = 42 cells, including prev/next-month padding).
struct DayCell {
    int year = 0, month = 0, day = 0;
    bool inCurrentMonth = false;
    bool isToday = false;
    std::string label;                    ///< Day-number text
    std::vector<std::string> eventTitles; ///< Event titles for the day (rendered directly by the View)
};

enum class CalendarError {
    None,
    FetchFailed,      ///< Network fetch failed
    ParseFailed,      ///< ICS parse failed
    InvalidData,      ///< Invalid subscription data (empty URL, etc.)
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
