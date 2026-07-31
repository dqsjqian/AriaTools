#include "viewmodels/SyncVm.h"

namespace wb::sync {

namespace {
constexpr const char* kTokenKey = "gitee.token";
}

SyncVm::SyncVm(wb::services::II18nService& i18n,
               wb::services::ISyncService& sync,
               wb::services::ISettingsService& settings,
               wb::services::ISecretStore& secrets)
    : wb::core::LocalizedVm(i18n, "sync"),
      log(""),
      syncNow([this] {
          auto r = sync_.sync(settings_.sync(), token_(),
                              [this](const std::string& l) { append_log_(l); });
          status.set(r.message);
      }),
      pushOnly([this] {
          auto r = sync_.push(settings_.sync(), token_(),
                              [this](const std::string& l) { append_log_(l); });
          status.set(r.message);
      }),
      pullOnly([this] {
          auto r = sync_.pull(settings_.sync(), token_(),
                              [this](const std::string& l) { append_log_(l); });
          status.set(r.message);
      }),
      sync_(sync),
      settings_(settings),
      secrets_(secrets)
{
    bind_text(title,     "title");
    bind_text(hint,      "hint");
    bind_text(syncLabel, "sync_now");
    bind_text(pullLabel, "pull");
    bind_text(pushLabel, "push");
    status.set(tr("ready"));
}

void SyncVm::on_activate() {}
void SyncVm::on_deactivate() { bag().clear(); }

void SyncVm::append_log_(const std::string& line) {
    log.set(log.get() + line + "\n");
}

std::string SyncVm::token_() const {
    if (auto tk = secrets_.retrieve(kTokenKey)) return *tk;
    return "";
}

}  // namespace wb::sync
