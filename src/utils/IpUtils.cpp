// ============================================================
// src/utils/IpUtils.cpp
// ============================================================
#include "utils/IpUtils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <array>

namespace ipinv::utils {

std::string detectIpType(const std::string& ip) {
    unsigned char buf[16];
    if (inet_pton(AF_INET, ip.c_str(), buf) == 1)  return "IPv4";
    if (inet_pton(AF_INET6, ip.c_str(), buf) == 1) return "IPv6";
    return "";
}

std::string normaliseIp(const std::string& ip) {
    unsigned char buf[16];
    char out[INET6_ADDRSTRLEN];
    if (inet_pton(AF_INET6, ip.c_str(), buf) == 1) {
        inet_ntop(AF_INET6, buf, out, sizeof(out));
        return std::string(out);
    }
    if (inet_pton(AF_INET, ip.c_str(), buf) == 1) {
        inet_ntop(AF_INET, buf, out, sizeof(out));
        return std::string(out);
    }
    return ip;
}

} // namespace ipinv::utils
