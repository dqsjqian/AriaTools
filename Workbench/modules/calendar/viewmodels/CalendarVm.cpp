#include "viewmodels/CalendarVm.h"

#include "infra/i18n/I18n.h"

#include <cstdio>

namespace wb::calendar {

CalendarVm::CalendarVm(std::shared_ptr<CalendarModel> model)
    : days(model->days),
      subscriptions(model->subscriptions),
      monthTitle(""),
      subscribeUrl(""),
      prevMonth([this] { model_->prev_month(); refresh_month_title_(); }),
      nextMonth([this] { model_->next_month(); refresh_month_title_(); }),
      today([this] { model_->go_today(); refresh_month_title_(); }),
      refresh([this] { (void)model_->refresh_all(); }),
      addSubscription([this] {
          if (!subscribeUrl.get().empty()) {
              if (model_->add_subscription(subscribeUrl.get(), {})) {
                  subscribeUrl.set("");
              }
          }
      }),
      removeSubscription([this](const std::string& id) {
          (void)model_->remove_subscription(id);
      }),
      model_(std::move(model))
{
    // Text: write freely here; text() sets the initial value immediately and auto-refreshes on language switch.
    text(title,          "title");
    text(hint,           "hint");
    text(urlPlaceholder, "url_placeholder");
    text(subscribeLabel, "subscribe");
    text(prevLabel,      "prev");
    text(nextLabel,      "next");
    text(todayLabel,     "today");
    text(refreshLabel,   "refresh");
    text(wd1, "wd_mon");
    text(wd2, "wd_tue");
    text(wd3, "wd_wed");
    text(wd4, "wd_thu");
    text(wd5, "wd_fri");
    text(wd6, "wd_sat");
    text(wd7, "wd_sun");

    // Current-month event count text: refreshes on language switch (localize) and on count change.
    localize([this] {
        status.set(std::to_string(model_->monthEventCount.get()) + " " +
                   wb::i18n::str("events_suffix"));
    });
    track(model_->monthEventCount.on_changed([this](int n) {
        status.set(std::to_string(n) + " " + wb::i18n::str("events_suffix"));
    }));

    track(model_->year.on_changed([this](int) { refresh_month_title_(); }));
    track(model_->month.on_changed([this](int) { refresh_month_title_(); }));
    refresh_month_title_();
}

void CalendarVm::on_activate() {
    model_->initialize();
    refresh_month_title_();
}

void CalendarVm::on_deactivate() { bag().clear(); }

void CalendarVm::refresh_month_title_() {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04d-%02d", model_->year.get(), model_->month.get());
    monthTitle.set(buf);
}


std::string CalendarVm::display_events(const DayCell& c) const {
    std::string s;
    int shown = 0;
    for (const auto& t : c.eventTitles) {
        if (shown >= 3) { s += "…"; break; }  // …
        if (!s.empty()) s += "\n";
        s += t;
        ++shown;
    }
    return s;
}

std::string CalendarVm::display_sub_name(const Subscription& s) const {
    return s.name.empty() ? s.url : s.name;
}

}  // namespace wb::calendar
