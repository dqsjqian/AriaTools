#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

CalendarVm::CalendarVm(wb::services::II18nService& i18n)
    : wb::core::LocalizedVm(i18n, "calendar"),
      subscribeUrl(""),
      addSubscription([this] {
          if (!subscribeUrl.get().empty()) {
              ++subCount_;
              subscribeUrl.set("");
              refresh_status_();
          }
      })
{
    bind_text(title, "title");
    bind_text(hint, "hint");
    bind_text(urlPlaceholder, "url_placeholder");
    bind_text(subscribeLabel, "subscribe");
    track(i18n.language().on_changed([this](const std::string&) { refresh_status_(); }));
    refresh_status_();
}

void CalendarVm::on_activate() {}
void CalendarVm::on_deactivate() { bag().clear(); }

void CalendarVm::refresh_status_() {
    if (subCount_ == 0) {
        status.set(tr("empty"));
    } else {
        status.set(tr("added_prefix") + std::to_string(subCount_) + tr("added_suffix"));
    }
}

}  // namespace wb::calendar
