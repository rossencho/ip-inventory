#pragma once
// ============================================================
// include/controllers/TerminateIpController.h
// POST /ip-inventory/terminate-ip-serviceId
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void terminateIp(const Req&, Cb&&);

} // namespace ipinv::controllers
