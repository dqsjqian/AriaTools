#pragma once
//
// CalendarModel — Module-level shared business state hub for the calendar module (not a per-View VM).
//
// Responsibilities:
//   * Holds subscription list, current year/month, the 42 DayCells of the current month (with prev/next-month padding and today highlight), and events;
//   * Fetches/parses .ics via CalendarService and classifies events into cells by date;
//   * Prev/next month, go to today, add/remove subscription, refresh.
//
// All date arithmetic (first weekday of month, padding, today detection, event classification) is done here; the View only renders.
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

    // Reactive state: bound directly by the VM.
    aria::ObservableList<Subscription> subscriptions;
    aria::ObservableList<DayCell>      days;      ///< Fixed 42 cells
    aria::Property<int>                year{0};
    aria::Property<int>                month{0};   ///< 1..12
    aria::Property<int>                monthEventCount{0};  ///< Current-month event count (text is localized by the VM)
    aria::Property<std::string>        lastError{""};

    // Initialize: locate the month containing today, load subscriptions and cached events, and lay out the grid.
    void initialize();

    // Navigation
    void prev_month();
    void next_month();
    void go_today();

    // Subscriptions
    bool add_subscription(const std::string& url, const std::string& name);
    bool remove_subscription(const SubId& id);

    // Fetch all subscriptions (online) -> aggregate events -> re-layout current month.
    bool refresh_all();

private:
    void rebuild_days_();                 ///< Builds the 42 cells from year/month + events_
    void set_error_(CalendarError e, std::string msg);
    static int weekday_of_(int y, int m, int d);   ///< 0=Monday .. 6=Sunday
    static int days_in_month_(int y, int m);

    std::shared_ptr<CalendarService> service_;
    EventList events_;                    ///< Aggregated events from all subscriptions (full set; filtered to current month on rebuild)
};

}  // namespace wb::calendar
