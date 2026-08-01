#pragma once
//
// 稳定服务工厂声明（platform 内部使用）。ServiceHub 用它们构造具体实现，
// 再 register_instance 进 DI Container。业务阶段把 stub 换成 real 只改这里 + ServiceHub。
//
#include "infra/i18n/II18nService.h"
#include "infra/storage/IStorageService.h"
#include "infra/secret/ISecretStore.h"
#include "infra/crypto/ICryptoService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/sync/ISyncService.h"
#include "infra/http/IHttpClient.h"

#include <string>

namespace wb::services {

ISettingsService* make_in_memory_settings_service();
IStorageService*  make_local_file_storage_service(std::string dataDir);
ISyncService*     make_stub_sync_service();
ISecretStore*     make_stub_secret_store();
ICryptoService*   make_stub_crypto_service();
IHttpClient*      make_stub_http_client();
#if WB_HAVE_CURL_HTTP
IHttpClient*      make_curl_http_client(std::string caBundlePath);
#endif
#if WB_HAVE_APPLE_CRYPTO
ICryptoService*   make_apple_crypto_service();
#endif
II18nService*     make_xml_i18n_service(std::string baseDir, std::string initialLang);

}  // namespace wb::services
