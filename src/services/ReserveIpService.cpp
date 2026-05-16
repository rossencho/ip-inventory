// ============================================================
// src/services/ReserveIpService.cpp
// Reserves a free IP address for a given serviceId.
// ============================================================
#include "services/ReserveIpService.h"
#include "repository/IpRepository.h"
#include "common/Types.h"
#include <drogon/drogon.h>
#include <memory>

using namespace drogon;
using namespace drogon::orm;
using namespace ipinv;

namespace ipinv::services {

void reserveIp(const std::string& serviceId,
               const std::string& ipType,
               Callback cb)
{
    bool wantV4 = (ipType == "IPv4" || ipType == "Both");
    bool wantV6 = (ipType == "IPv6" || ipType == "Both");

    if (!wantV4 && !wantV6) {
        cb(errorResponse(k400BadRequest, "Invalid ipType value"));
        return;
    }

    struct State {
        std::vector<IpRecord>                    results;
        int                                       pending;
        bool                                      failed{false};
        std::string                               error;
        Callback                                  cb;
        std::string                               serviceId;
        std::shared_ptr<drogon::orm::Transaction> txn;
    };

    auto state       = std::make_shared<State>();
    state->cb        = cb;
    state->serviceId = serviceId;
    state->pending   = (wantV4 ? 1 : 0) + (wantV6 ? 1 : 0);

    auto finalise = [](std::shared_ptr<State> st) {
        if (st->pending > 0) return;
        if (st->failed) {
            st->txn->rollback();
            st->txn.reset();
            st->cb(errorResponse(k409Conflict, st->error));
            return;
        }
        auto results = st->results;
        auto cbCopy  = st->cb;
        st->txn->setCommitCallback([results, cbCopy](bool committed) {
            if (!committed) {
                cbCopy(errorResponse(k500InternalServerError, "Transaction commit failed"));
                return;
            }
            Json::Value body;
            Json::Value arr(Json::arrayValue);
            for (const auto& rec : results) {
                Json::Value item;
                item["ip"]     = rec.ip;
                item["ipType"] = rec.ipType;
                arr.append(item);
            }
            body["ipAddresses"] = arr;
            cbCopy(okResponse(body));
        });
        st->txn.reset();
    };

    auto txn   = repository::client()->newTransaction();
    state->txn = txn;

    auto reserveOne = [state, finalise](const std::string& typeStr) {
        auto txn = state->txn;
        txn->execSqlAsync(
            "SELECT ip_address FROM ip_pool "
            "WHERE service_id=$1 AND ip_type=$2::ip_type_enum "
            "  AND status IN ('reserved','assigned') LIMIT 1",
            [state, typeStr, txn, finalise](const Result& existing) {
                if (!existing.empty()) {
                    state->results.push_back({
                        existing[0]["ip_address"].as<std::string>(), typeStr});
                    --state->pending;
                    finalise(state);
                    return;
                }
                txn->execSqlAsync(
                    "UPDATE ip_pool "
                    "SET status='reserved', service_id=$1, reserved_at=NOW() "
                    "WHERE id = ("
                    "  SELECT id FROM ip_pool "
                    "  WHERE status='free' AND ip_type=$2::ip_type_enum "
                    "  ORDER BY created_at LIMIT 1 FOR UPDATE SKIP LOCKED"
                    ") RETURNING ip_address",
                    [state, typeStr, finalise](const Result& r) {
                        if (r.empty()) {
                            state->failed = true;
                            state->error  = "No free " + typeStr + " address available";
                        } else {
                            repository::client()->execSqlAsync(
                                "INSERT INTO ip_audit_log(ip_address,action,service_id) "
                                "VALUES($1,'reserved',$2)",
                                [](const Result&) {},
                                [](const DrogonDbException& e) { LOG_WARN << "Audit: " << e.base().what(); },
                                r[0]["ip_address"].as<std::string>(),
                                state->serviceId
                            );
                            state->results.push_back({
                                r[0]["ip_address"].as<std::string>(), typeStr});
                        }
                        --state->pending;
                        finalise(state);
                    },
                    [state, finalise](const DrogonDbException& e) {
                        LOG_ERROR << "reserveIp UPDATE error: " << e.base().what();
                        state->failed = true;
                        state->error  = e.base().what();
                        --state->pending;
                        finalise(state);
                    },
                    state->serviceId,
                    typeStr
                );
            },
            [state, finalise](const DrogonDbException& e) {
                LOG_ERROR << "reserveIp SELECT error: " << e.base().what();
                state->failed = true;
                state->error  = e.base().what();
                --state->pending;
                finalise(state);
            },
            state->serviceId,
            typeStr
        );
    };

    if (wantV4) reserveOne("IPv4");
    if (wantV6) reserveOne("IPv6");
}

} // namespace ipinv::services
