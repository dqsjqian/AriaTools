#pragma once
//
// IHttpClient — Cross-platform HTTP(S) client service (infra stable service layer, zero UI / zero platform deps).
//
// Position: sibling to IStorageService / ICryptoService / ISyncService — network
// I/O is infrastructure; business code (e.g. calendar subscription fetch, future
// API calls) depends only on this interface and never writes network code in
// the View/platform layer.
//
// Implementation strategy: self-contained within the app. Vendored libcurl +
// mbedTLS statically compiled, all platforms (macOS/iOS/Win/Android/Linux)
// share one implementation and one CA bundle; users need not install any library.
//
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace wb::services {

enum class HttpMethod { Get, Post, Put, Delete, Patch, Head };

/// Request headers: a list of (name, value). Uses vector instead of map to preserve order and allow duplicate headers.
using HttpHeaders = std::vector<std::pair<std::string, std::string>>;

struct HttpRequest {
    HttpMethod method = HttpMethod::Get;
    std::string url;
    HttpHeaders headers;      ///< Extra request headers (e.g. Authorization, Content-Type)
    std::string body;         ///< Request body (for POST/PUT/PATCH; bytes carried as string)
    int timeoutSec = 30;      ///< Overall timeout (seconds); <=0 uses default
    bool followRedirects = true;
};

struct HttpResponse {
    bool ok = false;          ///< Transport succeeded and status is 2xx
    long status = 0;          ///< HTTP status code
    HttpHeaders headers;      ///< Response headers
    std::string body;         ///< Response body (bytes carried as string)
    std::string contentType;  ///< Content-Type (convenience extraction; equivalent to the same-named entry in headers)
    std::string error;        ///< Human-readable error message on failure

    /// Convenience: get a response header (case-insensitive); returns empty string if absent.
    [[nodiscard]] std::string header(std::string_view name) const;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    /// Generic request entry point. TLS certificates verified against the bundled CA bundle.
    [[nodiscard]] virtual HttpResponse send(const HttpRequest& request) = 0;

    // ── Convenience methods (default implementations based on send; subclasses need not override) ──
    [[nodiscard]] HttpResponse get(const std::string& url,
                                   HttpHeaders headers = {}) {
        return send({HttpMethod::Get, url, std::move(headers), {}, 30, true});
    }

    [[nodiscard]] HttpResponse post(const std::string& url,
                                    std::string body,
                                    HttpHeaders headers = {}) {
        return send({HttpMethod::Post, url, std::move(headers), std::move(body), 30, true});
    }

    [[nodiscard]] HttpResponse put(const std::string& url,
                                   std::string body,
                                   HttpHeaders headers = {}) {
        return send({HttpMethod::Put, url, std::move(headers), std::move(body), 30, true});
    }

    [[nodiscard]] HttpResponse del(const std::string& url,
                                   HttpHeaders headers = {}) {
        return send({HttpMethod::Delete, url, std::move(headers), {}, 30, true});
    }
};

}  // namespace wb::services
