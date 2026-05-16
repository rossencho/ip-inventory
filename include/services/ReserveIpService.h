#pragma once
#include <drogon/drogon.h>
#include <string>
#include "common/Types.h"

namespace ipinv::services {
using Callback = std::function<void(drogon::HttpResponsePtr)>;
void reserveIp(const std::string& serviceId, const std::string& ipType, Callback cb);
} // namespace ipinv::services
