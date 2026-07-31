#include "viewmodels/SettingsVm.h"

namespace wb::settings {

namespace {
constexpr const char* kTokenKey = "gitee.token";
}

SettingsVm::SettingsVm(wb::services::II18nService& i18n,
                       wb::services::ISettingsService& settings,
                       wb::services::ISecretStore& secrets)
    : wb::core::LocalizedVm(i18n, "settings"),
      dataDir(""),
      remoteUrl(""),
      branch("main"),
      username(""),
      token(""),
      autoSync(true),
      encryptScope(0),
      language(i18n.language().get()),
      save([this] { save_to_service_(); }),
      switchLanguage([this](const std::string& lang) {
          this->i18n().set_language(lang);
          language.set(lang);
      }),
      settings_(settings),
      secrets_(secrets)
{
    bind_text(title,         "title");
    bind_text(hint,          "hint");
    bind_text(dataDirLabel,  "data_dir");
    bind_text(remoteLabel,   "remote");
    bind_text(branchLabel,   "branch");
    bind_text(usernameLabel, "username");
    bind_text(tokenLabel,    "token");
    bind_text(autoSyncLabel, "auto_sync");
    bind_text(languageLabel, "language");
    bind_text(saveLabel,     "save");

    load_from_service_();
}

void SettingsVm::on_activate() { load_from_service_(); }
void SettingsVm::on_deactivate() { bag().clear(); }

void SettingsVm::load_from_service_() {
    const auto& s = settings_.sync();
    dataDir.set(s.dataDir);
    remoteUrl.set(s.remoteUrl);
    branch.set(s.branch);
    username.set(s.username);
    autoSync.set(s.autoSync);
    encryptScope.set(static_cast<int>(s.encryptScope));
    language.set(i18n().language().get());
    if (auto tk = secrets_.retrieve(kTokenKey)) token.set(*tk);
}

void SettingsVm::save_to_service_() {
    auto& s = settings_.sync();
    s.dataDir   = dataDir.get();
    s.remoteUrl = remoteUrl.get();
    s.branch    = branch.get();
    s.username  = username.get();
    s.autoSync  = autoSync.get();
    s.encryptScope = static_cast<wb::services::EncryptScope>(encryptScope.get());
    settings_.set_language(language.get());
    settings_.save();

    if (!token.get().empty()) secrets_.store(kTokenKey, token.get());

    status.set(tr("saved"));
}

}  // namespace wb::settings
