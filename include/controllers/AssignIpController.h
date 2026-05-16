#pragma once
// ============================================================
// include/controllers/AssignIpController.h
// POST /ip-inventory/assign-ip-serviceId
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void assignIp(const Req&, Cb&&);

} // namespace ipinv::controllers
