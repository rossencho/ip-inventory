#pragma once
#include <drogon/drogon.h>
#include <string>
#include "common/Types.h"

namespace ipinv::services {
using Callback = std::function<void(drogon::HttpResponsePtr)>;
void getByServiceId(const std::string& serviceId, Callback cb);
} // namespace ipinv::services
