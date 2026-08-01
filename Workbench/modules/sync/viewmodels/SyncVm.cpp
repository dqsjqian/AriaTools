#include "viewmodels/SyncVm.h"

#include "infra/i18n/I18n.h"

namespace wb::sync {

namespace {
constexpr const char* kTokenKey = "gitee.token";
}

SyncVm::SyncVm(wb::services::ISyncService& sync,
               wb::services::ISettingsService& settings,
               wb::services::ISecretStore& secrets)
    : dataDir(""),
      remoteUrl(""),
      branch("main"),
      username(""),
      token(""),
      autoSync(true),
      encryptScope(0),
      log(""),
      saveConfig([this] { save_config_(); }),
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
    text(title,         "title");
    text(hint,          "hint");
    text(dataDirLabel,  "data_dir");
    text(remoteLabel,   "remote");
    text(branchLabel,   "branch");
    text(usernameLabel, "username");
    text(tokenLabel,    "token");
    text(autoSyncLabel, "auto_sync");
    text(saveLabel,     "save_config");
    text(syncLabel,     "sync_now");
    text(pullLabel,     "pull");
    text(pushLabel,     "push");
    localize([this] { status.set(wb::i18n::str("ready")); });

    load_config_();
}

void SyncVm::on_activate() { load_config_(); }
void SyncVm::on_deactivate() { bag().clear(); }

void SyncVm::load_config_() {
    const auto& s = settings_.sync();
    dataDir.set(s.dataDir);
    remoteUrl.set(s.remoteUrl);
    branch.set(s.branch);
    username.set(s.username);
    autoSync.set(s.autoSync);
    encryptScope.set(static_cast<int>(s.encryptScope));
    if (auto tk = secrets_.retrieve(kTokenKey)) token.set(*tk);
}

void SyncVm::save_config_() {
    auto& s = settings_.sync();
    s.dataDir     = dataDir.get();
    s.remoteUrl   = remoteUrl.get();
    s.branch      = branch.get();
    s.username    = username.get();
    s.autoSync    = autoSync.get();
    s.encryptScope = static_cast<wb::services::EncryptScope>(encryptScope.get());
    settings_.save();
    if (!token.get().empty()) secrets_.store(kTokenKey, token.get());
    status.set(wb::i18n::str("config_saved"));
}

void SyncVm::append_log_(const std::string& line) {
    log.set(log.get() + line + "\n");
}

std::string SyncVm::token_() const {
    if (auto tk = secrets_.retrieve(kTokenKey)) return *tk;
    return "";
}

}  // namespace wb::sync
