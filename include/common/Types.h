#pragma once
// ============================================================
// include/common/Types.h
// Shared types and JSON response helpers
// ============================================================
#include <drogon/drogon.h>
#include <json/json.h>
#include <string>
#include <vector>
#include <optional>

namespace ipinv {

// ── IP address record ────────────────────────────────────────
struct IpRecord {
    std::string ip;
    std::string ipType;  // "IPv4" | "IPv6"
};

// ── Standard success/error response ─────────────────────────
inline Json::Value makeStatus(const std::string& code,
                               const std::string& message) {
    Json::Value v;
    v["statusCode"]    = code;
    v["statusMessage"] = message;
    return v;
}

inline drogon::HttpResponsePtr okResponse(const Json::Value& body) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(drogon::k200OK);
    return resp;
}

inline drogon::HttpResponsePtr errorResponse(drogon::HttpStatusCode code,
                                              const std::string& msg) {
    Json::Value body;
    body["statusCode"]    = std::to_string(static_cast<int>(code));
    body["statusMessage"] = msg;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(code);
    return resp;
}

inline drogon::HttpResponsePtr ok() {
    return okResponse(makeStatus("0", "Successful operation. OK"));
}

} // namespace ipinv
