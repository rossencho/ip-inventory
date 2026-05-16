#pragma once
// ============================================================
// include/controllers/IpPoolController.h
// POST /ip-inventory/ip-pool
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::controllers {

using Req = drogon::HttpRequestPtr;
using Cb  = std::function<void(const drogon::HttpResponsePtr&)>;

void ipPool(const Req&, Cb&&);

} // namespace ipinv::controllers
