#pragma once
// ============================================================
// include/controllers/ServiceIdQueryController.h
// GET /ip-inventory/serviceId?serviceId=xxx
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void getServiceId(const Req&, Cb&&);

} // namespace ipinv::controllers
