// ============================================================
// src/services/ReservationExpiryJob.cpp
// ============================================================
#include "services/ReservationExpiryJob.h"
#include "repository/IpRepository.h"
#include <drogon/drogon.h>

using namespace drogon::orm;

namespace ipinv::services {

void ReservationExpiryJob::start(int intervalSeconds, int timeoutSeconds) {
    LOG_INFO << "[ReservationExpiryJob] Started: interval=" << intervalSeconds
             << "s, timeout=" << timeoutSeconds << "s";
    drogon::app().getLoop()->runEvery(
        static_cast<double>(intervalSeconds),
        [timeoutSeconds]() { runOnce(timeoutSeconds); }
    );
}

void ReservationExpiryJob::runOnce(int timeoutSeconds) {
    auto db = ipinv::repository::client();
    db->execSqlAsync(
        "UPDATE ip_pool "
        "SET status='free', service_id=NULL, reserved_at=NULL "
        "WHERE status='reserved' "
        "  AND reserved_at < NOW() - ($1 || ' seconds')::INTERVAL "
        "RETURNING ip_address, service_id",
        [db](const Result& r) {
            if (r.empty()) return;
            LOG_INFO << "[ReservationExpiryJob] Released " << r.size() << " expired reservation(s)";
            for (const auto& row : r) {
                std::string ip  = row["ip_address"].as<std::string>();
                std::string sid = row["service_id"].isNull() ? "" : row["service_id"].as<std::string>();
                db->execSqlAsync(
                    "INSERT INTO ip_audit_log(ip_address,action,service_id,details) "
                    "VALUES($1,'released',$2,'{\"reason\":\"timeout\"}'::jsonb)",
                    [](const Result&) {},
                    [](const DrogonDbException& e) { LOG_WARN << "Audit: " << e.base().what(); },
                    ip, sid
                );
            }
        },
        [](const DrogonDbException& e) {
            LOG_ERROR << "[ReservationExpiryJob] DB error: " << e.base().what();
        },
        std::to_string(timeoutSeconds)
    );
}

} // namespace ipinv::services
