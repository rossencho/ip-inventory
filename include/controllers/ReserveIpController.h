#pragma once
// ============================================================
// include/controllers/ReserveIpController.h
// POST /ip-inventory/reserve-ip
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void reserveIp(const Req&, Cb&&);

} // namespace ipinv::controllers
