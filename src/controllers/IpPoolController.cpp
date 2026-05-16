// ============================================================
// src/controllers/IpPoolController.cpp
// POST /ip-inventory/ip-pool
// Adds IP addresses to the inventory pool.
// ============================================================
#include "controllers/IpPoolController.h"
#include "services/IpPoolService.h"
#include "utils/JsonValidator.h"
#include "utils/IpUtils.h"
#include "common/Types.h"

namespace iu  = ipinv::utils;
namespace svc = ipinv::services;
using ipinv::IpRecord;
using ipinv::errorResponse;

namespace ipinv::controllers {

void ipPool(const Req& req, Cb&& cb)
{
    auto body = req->getJsonObject();
    if (!body) { cb(errorResponse(drogon::k400BadRequest, "Invalid or missing JSON body")); return; }
    if (auto e = iu::requireArray(*body, "ipAddresses"); e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }

    std::vector<IpRecord> records;
    for (const auto& item : (*body)["ipAddresses"]) {
        if (!item.isMember("ip") || !item["ip"].isString()) {
            cb(errorResponse(drogon::k400BadRequest, "Each entry must have a string 'ip' field")); return;
        }
        if (!item.isMember("ipType") || !item["ipType"].isString()) {
            cb(errorResponse(drogon::k400BadRequest, "Each entry must have a string 'ipType' field")); return;
        }
        std::string ip     = iu::normaliseIp(item["ip"].asString());
        std::string ipType = item["ipType"].asString();
        if (ipType != "IPv4" && ipType != "IPv6") {
            cb(errorResponse(drogon::k400BadRequest, "Invalid ipType: " + ipType)); return;
        }
        std::string detected = iu::detectIpType(ip);
        if (detected.empty()) { cb(errorResponse(drogon::k400BadRequest, "Invalid IP: " + ip)); return; }
        if (detected != ipType) { cb(errorResponse(drogon::k400BadRequest, ip + " does not match " + ipType)); return; }
        records.push_back({ip, ipType});
    }
    svc::addIpPool(records, std::move(cb));
}

} // namespace ipinv::controllers
