#pragma once
#include <drogon/drogon.h>
#include <string>
#include <vector>
#include "common/Types.h"

namespace ipinv::services {
using Callback = std::function<void(drogon::HttpResponsePtr)>;
void assignIp(const std::string& serviceId, const std::vector<std::string>& ips, Callback cb);
} // namespace ipinv::services
