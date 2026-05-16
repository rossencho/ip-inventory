// ============================================================
// src/controllers/AssignIpController.cpp
// POST /ip-inventory/assign-ip-serviceId
// Assigns reserved or free IPs to a serviceId.
// ============================================================
#include "controllers/AssignIpController.h"
#include "services/AssignIpService.h"
#include "utils/JsonValidator.h"
#include "utils/IpUtils.h"
#include "common/Types.h"

namespace iu  = ipinv::utils;
namespace svc = ipinv::services;
using ipinv::errorResponse;

namespace ipinv::controllers {

void assignIp(const Req& req, Cb&& cb)
{
    auto body = req->getJsonObject();
    if (!body) { cb(errorResponse(drogon::k400BadRequest, "Invalid or missing JSON body")); return; }
    if (auto e = iu::requireString(*body, "serviceId");   e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }
    if (auto e = iu::requireArray (*body, "ipAddresses"); e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }

    std::string serviceId = (*body)["serviceId"].asString();
    std::vector<std::string> ips;
    for (const auto& item : (*body)["ipAddresses"]) {
        if (!item.isMember("ip") || !item["ip"].isString()) {
            cb(errorResponse(drogon::k400BadRequest, "Each entry must have a string 'ip' field")); return;
        }
        std::string ip = iu::normaliseIp(item["ip"].asString());
        if (iu::detectIpType(ip).empty()) { cb(errorResponse(drogon::k400BadRequest, "Invalid IP: " + ip)); return; }
        ips.push_back(ip);
    }
    svc::assignIp(serviceId, ips, std::move(cb));
}

} // namespace ipinv::controllers
