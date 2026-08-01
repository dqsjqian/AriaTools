//
// HttpResponse backend-agnostic helper implementation (shared by stub / curl implementations).
//
#include "infra/http/IHttpClient.h"

#include <algorithm>
#include <cctype>

namespace wb::services {

namespace {
bool iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}
}  // namespace

std::string HttpResponse::header(std::string_view name) const {
    for (const auto& [k, v] : headers) {
        if (iequals(k, name)) return v;
    }
    return {};
}

}  // namespace wb::services
