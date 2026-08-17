#pragma once
//
// CalendarVm — Calendar view model. Only maintains page reactive state and Commands;
// month-grid layout / subscription fetching is fully delegated to CalendarModel; View only binds the VM.
// Text is bound via the global wb::i18n facade (no need to inherit an i18n base class or inject II18nService).
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "models/CalendarModel.h"

#include <memory>

namespace wb::calendar {

class CalendarVm final : public wb::core::BaseVm {
public:
    explicit CalendarVm(std::shared_ptr<CalendarModel> model);

    // Month data (forwarded from Model, for View binding).
    aria::ObservableList<DayCell>&      days;
    aria::ObservableList<Subscription>& subscriptions;
    aria::Property<std::string>         monthTitle;   ///< "2026-07" (dynamic)
    aria::Property<std::string>         status{""};   ///< Localized text for current-month event count

    // Subscription input
    aria::Property<std::string> subscribeUrl;

    // Commands
    aria::Command<>            prevMonth;
    aria::Command<>            nextMonth;
    aria::Command<>            today;
    aria::Command<>            refresh;
    aria::Command<>            addSubscription;
    aria::Command<std::string> removeSubscription;  ///< Argument: subscription id

    // Text (updates on language switch)
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> urlPlaceholder;
    aria::Property<std::string> subscribeLabel;
    aria::Property<std::string> prevLabel;
    aria::Property<std::string> nextLabel;
    aria::Property<std::string> todayLabel;
    aria::Property<std::string> refreshLabel;
    // Weekday header (Mon..Sun)
    aria::Property<std::string> wd1, wd2, wd3, wd4, wd5, wd6, wd7;

    /// Day-cell events text (≤3 titles + ellipsis) — display assembly lives here.
    [[nodiscard]] std::string display_events(const DayCell& c) const;
    /// Subscription display name (falls back to URL when unset).
    [[nodiscard]] std::string display_sub_name(const Subscription& s) const;
    void on_activate() override;

private:
    void refresh_month_title_();

    std::shared_ptr<CalendarModel> model_;
};

}  // namespace wb::calendar
