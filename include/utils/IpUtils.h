#pragma once
// ============================================================
// include/utils/IpUtils.h
// ============================================================
#include <string>

namespace ipinv::utils {

// Returns "IPv4", "IPv6", or "" (invalid)
std::string detectIpType(const std::string& ip);

// Normalise IPv6 to canonical form (lower-case, compressed)
std::string normaliseIp(const std::string& ip);

} // namespace ipinv::utils
