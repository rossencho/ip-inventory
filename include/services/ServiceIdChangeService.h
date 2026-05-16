#pragma once
#include <drogon/drogon.h>
#include <string>
#include "common/Types.h"

namespace ipinv::services {
using Callback = std::function<void(drogon::HttpResponsePtr)>;
void changeServiceId(const std::string& oldServiceId, const std::string& newServiceId, Callback cb);
} // namespace ipinv::services
