#pragma once
#include <drogon/drogon.h>
#include <vector>
#include "common/Types.h"

namespace ipinv::services {
using Callback = std::function<void(drogon::HttpResponsePtr)>;
void addIpPool(const std::vector<IpRecord>& records, Callback cb);
} // namespace ipinv::services
