// ============================================================
// src/controllers/ReserveIpController.cpp
// POST /ip-inventory/reserve-ip
// Reserves a free IP address for a given serviceId.
// ============================================================
#include "controllers/ReserveIpController.h"
#include "services/ReserveIpService.h"
#include "utils/JsonValidator.h"
#include "common/Types.h"

namespace iu  = ipinv::utils;
namespace svc = ipinv::services;
using ipinv::errorResponse;

namespace ipinv::controllers {

void reserveIp(const Req& req, Cb&& cb)
{
    auto body = req->getJsonObject();
    if (!body) { cb(errorResponse(drogon::k400BadRequest, "Invalid or missing JSON body")); return; }
    if (auto e = iu::requireString(*body, "serviceId"); e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }
    if (auto e = iu::requireString(*body, "ipType");    e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }

    std::string serviceId = (*body)["serviceId"].asString();
    std::string ipType    = (*body)["ipType"].asString();
    if (!iu::isValidIpType(ipType)) {
        cb(errorResponse(drogon::k400BadRequest, "ipType must be IPv4, IPv6, or Both")); return;
    }
    svc::reserveIp(serviceId, ipType, std::move(cb));
}

} // namespace ipinv::controllers
