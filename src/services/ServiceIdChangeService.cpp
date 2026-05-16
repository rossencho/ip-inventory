// ============================================================
// src/services/ServiceIdChangeService.cpp
// Re-assigns all IPs from one serviceId to another.
// ============================================================
#include "services/ServiceIdChangeService.h"
#include "repository/IpRepository.h"
#include "common/Types.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;
using namespace ipinv;

namespace ipinv::services {

void changeServiceId(const std::string& oldServiceId,
                     const std::string& newServiceId,
                     Callback cb)
{
    auto dbClient = repository::client();
    dbClient->execSqlAsync(
        "UPDATE ip_pool SET service_id=$1 "
        "WHERE service_id=$2 AND status='assigned' "
        "RETURNING ip_address",
        [cb, oldServiceId, newServiceId, dbClient](const Result& r) {
            if (r.empty()) {
                cb(errorResponse(k404NotFound,
                    "No assigned IPs found for serviceId: " + oldServiceId));
                return;
            }
            for (const auto& row : r) {
                dbClient->execSqlAsync(
                    "INSERT INTO ip_audit_log"
                    "(ip_address,action,service_id,old_service_id) "
                    "VALUES($1,'service_changed',$2,$3)",
                    [](const Result&) {},
                    [](const DrogonDbException& e) { LOG_WARN << "Audit: " << e.base().what(); },
                    row["ip_address"].as<std::string>(),
                    newServiceId,
                    oldServiceId
                );
            }
            cb(ok());
        },
        [cb](const DrogonDbException& e) {
            LOG_ERROR << "changeServiceId DB error: " << e.base().what();
            cb(errorResponse(k500InternalServerError, e.base().what()));
        },
        newServiceId,
        oldServiceId
    );
}

} // namespace ipinv::services
