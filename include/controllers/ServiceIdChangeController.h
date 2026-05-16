#pragma once
// ============================================================
// include/controllers/ServiceIdChangeController.h
// POST /ip-inventory/serviceId-change
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void serviceChange(const Req&, Cb&&);

} // namespace ipinv::controllers
