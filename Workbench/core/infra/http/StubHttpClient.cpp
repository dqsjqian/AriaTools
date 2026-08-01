//
// StubHttpClient — Placeholder HTTP implementation.
// Keeps DI wiring complete and the app buildable until vendored libcurl + mbedTLS are integrated.
// send() returns a clear "network backend not configured" error and performs no real network access.
// Real implementation: CurlHttpClient (task: vendoring curl+mbedTLS).
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
