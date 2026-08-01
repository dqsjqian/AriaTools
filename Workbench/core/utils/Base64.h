#pragma once
//
// wb_utils/Base64 — Stateless pure functions (Base64 encode/decode). Zero business semantics, zero platform dependencies.
//
#include <string>

namespace wb::utils {

std::string base64_encode(const std::string& in);
std::string base64_decode(const std::string& in);

}  // namespace wb::utils
