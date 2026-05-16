#pragma once
// ============================================================
// include/repository/IpRepository.h
// Data access layer — owns the PostgreSQL client.
// The rest of the codebase calls repository::client() to get
// a shared async DB connection; it never touches Drogon's
// internal client registry directly.
// ============================================================
#include <drogon/orm/DbClient.h>
#include <stdexcept>

namespace ipinv::repository {

// Returns a reference to the global DB client pointer.
// Populated once by AppConfig::initDbClient() before app().run().
inline drogon::orm::DbClientPtr& globalClient() {
    static drogon::orm::DbClientPtr s_client;
    return s_client;
}

// Safe accessor — throws if not yet initialised.
inline drogon::orm::DbClientPtr client() {
    auto& c = globalClient();
    if (!c) throw std::runtime_error("DB client not initialised");
    return c;
}

} // namespace ipinv::repository
