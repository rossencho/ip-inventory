// ============================================================
// src/services/ServiceIdQueryService.cpp
// Returns all assigned IPs for a given serviceId.
// ============================================================
#include "services/ServiceIdQueryService.h"
#include "repository/IpRepository.h"
#include "common/Types.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;
using namespace ipinv;

namespace ipinv::services {

void getByServiceId(const std::string& serviceId, Callback cb) {
    auto dbClient = repository::client();
    dbClient->execSqlAsync(
        "SELECT ip_address, ip_type FROM ip_pool "
        "WHERE service_id=$1 AND status='assigned' "
        "ORDER BY ip_type, ip_address",
        [cb](const Result& r) {
            Json::Value body;
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value item;
                item["ip"]     = row["ip_address"].as<std::string>();
                item["ipType"] = row["ip_type"].as<std::string>();
                arr.append(item);
            }
            body["ipAddresses"] = arr;
            cb(okResponse(body));
        },
        [cb](const DrogonDbException& e) {
            LOG_ERROR << "getByServiceId DB error: " << e.base().what();
            cb(errorResponse(k500InternalServerError, e.base().what()));
        },
        serviceId
    );
}

} // namespace ipinv::services
