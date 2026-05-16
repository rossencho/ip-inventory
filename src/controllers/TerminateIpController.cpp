// ============================================================
// src/controllers/TerminateIpController.cpp
// POST /ip-inventory/terminate-ip-serviceId
// Releases assigned IPs back to free status.
// ============================================================
#include "controllers/TerminateIpController.h"
#include "services/TerminateIpService.h"
#include "utils/JsonValidator.h"
#include "utils/IpUtils.h"
#include "common/Types.h"

namespace iu  = ipinv::utils;
namespace svc = ipinv::services;
using ipinv::errorResponse;

namespace ipinv::controllers {

void terminateIp(const Req& req, Cb&& cb)
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
    svc::terminateIp(serviceId, ips, std::move(cb));
}

} // namespace ipinv::controllers
