#pragma once
//
// IHttpClient — 跨平台 HTTP(S) 客户端服务（infra 稳定服务层，零 UI / 零平台依赖）。
//
// 定位：与 IStorageService / ICryptoService / ISyncService 同构——网络 I/O 属于
// 基础设施，业务（如日历订阅拉取、未来的 API 调用）只依赖本接口，绝不在
// View/平台层写网络代码。
//
// 实现策略：app 内自闭环。用 vendored libcurl + mbedTLS 静态编译，所有平台
// （macOS/iOS/Win/Android/Linux）走同一实现与同一份 CA bundle，用户无需安装任何库。
//
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace wb::services {

enum class HttpMethod { Get, Post, Put, Delete, Patch, Head };

/// 请求头：一组 (name, value)。用 vector 而非 map，保留顺序、允许重复头。
using HttpHeaders = std::vector<std::pair<std::string, std::string>>;

struct HttpRequest {
    HttpMethod method = HttpMethod::Get;
    std::string url;
    HttpHeaders headers;      ///< 额外请求头（如 Authorization、Content-Type）
    std::string body;         ///< 请求体（POST/PUT/PATCH 用；字节以字符串承载）
    int timeoutSec = 30;      ///< 整体超时（秒），<=0 用默认
    bool followRedirects = true;
};

struct HttpResponse {
    bool ok = false;          ///< 传输成功且 status 属于 2xx
    long status = 0;          ///< HTTP 状态码
    HttpHeaders headers;      ///< 响应头
    std::string body;         ///< 响应体（字节以字符串承载）
    std::string contentType;  ///< Content-Type（便捷提取，等价于 headers 中同名项）
    std::string error;        ///< 失败时的可读错误信息

    /// 便捷：取某响应头（大小写不敏感），无则返回空串。
    [[nodiscard]] std::string header(std::string_view name) const;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    /// 通用请求入口。TLS 证书用打包的 CA bundle 校验。
    [[nodiscard]] virtual HttpResponse send(const HttpRequest& request) = 0;

    // ── 便捷方法（默认基于 send 实现，实现类无需重写）───────────────────────
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
