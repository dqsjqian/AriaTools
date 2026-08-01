#pragma once
//
// CalendarService — Private business service of the calendar module (pure C++, cross-platform).
//
// Responsibilities:
//   * ICS text parsing: VEVENT / SUMMARY / DTSTART / DTEND (RFC5545 line folding,
//     DATE and DATE-TIME, UTC 'Z'); MVP does not expand RRULE.
//   * Subscription persistence: one file per subscription calendar/subs/<id>.ini;
//     fetch cache calendar/cache/<id>.ics (both via IStorageService).
//   * refresh: fetch remote .ics via IHttpClient -> write cache -> parse.
//
// Both network and storage go through injected interfaces, so this service is
// platform-agnostic; VM/Model depend only on this service.
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

    // Subscription persistence
    [[nodiscard]] CalendarResult<SubscriptionList> load_subscriptions();
    [[nodiscard]] CalendarResult<Subscription> add_subscription(const std::string& url,
                                                                const std::string& name);
    [[nodiscard]] CalendarResult<void> remove_subscription(const SubId& id);

    // Fetch + parse: returns the event list for this subscription and updates cache and lastFetched.
    [[nodiscard]] CalendarResult<EventList> refresh(const Subscription& sub);

    // Read cache (offline): parses the cached .ics without network access.
    [[nodiscard]] CalendarResult<EventList> load_cached(const SubId& id);

    // Pure parse (called directly by tests, no IO).
    [[nodiscard]] static CalendarResult<EventList> parse_ics(std::string_view text,
                                                             const SubId& source);

private:
    [[nodiscard]] CalendarResult<void> save_subscription_(const Subscription& sub);

    wb::services::IStorageService& storage_;
    wb::services::IHttpClient& http_;
};

}  // namespace wb::calendar
