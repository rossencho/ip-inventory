// ============================================================
// src/services/TerminateIpService.cpp
// Releases assigned IPs back to free status.
// ============================================================
#include "services/TerminateIpService.h"
#include "repository/IpRepository.h"
#include "common/Types.h"
#include <drogon/drogon.h>
#include <atomic>
#include <memory>

using namespace drogon;
using namespace drogon::orm;
using namespace ipinv;

namespace ipinv::services {

void terminateIp(const std::string& serviceId,
                 const std::vector<std::string>& ips,
                 Callback cb)
{
    if (ips.empty()) {
        cb(errorResponse(k400BadRequest, "No IP addresses provided"));
        return;
    }

    auto total    = static_cast<int>(ips.size());
    auto done     = std::make_shared<std::atomic<int>>(0);
    auto matched  = std::make_shared<std::atomic<int>>(0);
    auto failed   = std::make_shared<std::atomic<bool>>(false);
    auto dbClient = repository::client();

    for (const auto& ip : ips) {
        dbClient->execSqlAsync(
            "UPDATE ip_pool SET status='free', service_id=NULL, "
            "  assigned_at=NULL, reserved_at=NULL "
            "WHERE ip_address=$1 AND service_id=$2 "
            "RETURNING ip_address",
            [cb, done, matched, total, failed, dbClient, serviceId, ip](const Result& r) {
                if (!r.empty()) {
                    matched->fetch_add(1);
                    dbClient->execSqlAsync(
                        "INSERT INTO ip_audit_log(ip_address,action,service_id) "
                        "VALUES($1,'terminated',$2)",
                        [](const Result&) {},
                        [](const DrogonDbException& e) { LOG_WARN << "Audit: " << e.base().what(); },
                        ip, serviceId
                    );
                }
                int n = done->fetch_add(1) + 1;
                if (n == total && !failed->load()) {
                    if (matched->load() != total)
                        cb(errorResponse(k409Conflict, "One or more IPs could not be terminated"));
                    else
                        cb(ok());
                }
            },
            [cb, done, total, failed](const DrogonDbException& e) {
                LOG_ERROR << "terminateIp DB error: " << e.base().what();
                if (!failed->exchange(true))
                    cb(errorResponse(k500InternalServerError, e.base().what()));
                done->fetch_add(1);
            },
            ip,
            serviceId
        );
    }
}

} // namespace ipinv::services
