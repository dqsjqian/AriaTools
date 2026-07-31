#pragma once
//
// wb_utils/Base64 — 无状态纯函数（Base64 编解码）。零业务语义、零平台依赖。
//
#include <string>

namespace wb::utils {

std::string base64_encode(const std::string& in);
std::string base64_decode(const std::string& in);

}  // namespace wb::utils
