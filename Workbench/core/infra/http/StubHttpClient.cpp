//
// StubHttpClient — 占位 HTTP 实现。
// 在 vendored libcurl + mbedTLS 接入前先让 DI 装配完整、app 可构建。
// send() 返回明确的"未配置网络后端"错误，不做任何真实网络访问。
// 真实实现见 CurlHttpClient（task: vendoring curl+mbedTLS）。
//
#include "infra/http/IHttpClient.h"
#include "infra/log/Log.h"

namespace wb::services {

namespace {

class StubHttpClient final : public IHttpClient {
public:
    HttpResponse send(const HttpRequest& request) override {
        HttpResponse r;
        r.ok = false;
        r.status = 0;
        r.error = "http backend not configured yet (url=" + request.url + ")";
        log_warn << "stub http: refusing " << request.url
                 << " (curl+mbedTLS backend not wired yet)";
        return r;
    }
};

}  // namespace

IHttpClient* make_stub_http_client() {
    return new StubHttpClient();
}

}  // namespace wb::services
