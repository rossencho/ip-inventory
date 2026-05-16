// ============================================================
// src/services/IpPoolService.cpp
// Adds IP addresses to the inventory pool.
// ============================================================
#include "services/IpPoolService.h"
#include "repository/IpRepository.h"
#include "common/Types.h"
#include <drogon/drogon.h>
#include <atomic>
#include <memory>

using namespace drogon;
using namespace drogon::orm;
using namespace ipinv;

namespace ipinv::services
{

    void addIpPool(const std::vector<IpRecord> &records, Callback cb)
    {
        if (records.empty())
        {
            cb(errorResponse(k400BadRequest, "No IP addresses provided"));
            return;
        }

        auto total = static_cast<int>(records.size());
        auto done = std::make_shared<std::atomic<int>>(0);
        auto failed = std::make_shared<std::atomic<bool>>(false);
        auto dbClient = repository::client();

        for (const auto &rec : records)
        {
            dbClient->execSqlAsync(
                "INSERT INTO ip_pool (ip_address, ip_type, status) "
                "VALUES ($1, $2::ip_type_enum, 'free') "
                "ON CONFLICT (ip_address) DO NOTHING",
                [cb, done, total, failed, dbClient, ip = rec.ip](const Result &)
                {
                    dbClient->execSqlAsync(
                        "INSERT INTO ip_audit_log(ip_address, action, details) "
                        "VALUES ($1, 'added', '{\"source\":\"ip-pool\"}'::jsonb)",
                        [](const Result &) {},
                        [](const DrogonDbException &e)
                        { LOG_WARN << "Audit: " << e.base().what(); },
                        ip);
                    if (done->fetch_add(1) + 1 == total && !failed->load())
                        cb(ok());
                },
                [cb, done, total, failed](const DrogonDbException &e)
                {
                    LOG_ERROR << "addIpPool DB error: " << e.base().what();
                    if (!failed->exchange(true))
                        cb(errorResponse(k500InternalServerError, e.base().what()));
                    done->fetch_add(1);
                },
                rec.ip,
                rec.ipType);
        }
    }

} // namespace ipinv::services
