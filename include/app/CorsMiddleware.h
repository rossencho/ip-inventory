#pragma once
// ============================================================
// include/app/CorsMiddleware.h
// Adds CORS headers to every response so the GUI can call the
// API from any origin (file://, localhost variants, etc.)
// ============================================================
#include <drogon/drogon.h>

namespace ipinv::app {

inline void registerCors() {
    // Handle preflight OPTIONS requests
    drogon::app().registerHandler(
        "/ip-inventory/{path}",
        [](const drogon::HttpRequestPtr& req,
           std::function<void(const drogon::HttpResponsePtr&)>&& cb,
           const std::string& path)
        {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->addHeader("Access-Control-Allow-Origin",  "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
            resp->setStatusCode(drogon::k204NoContent);
            cb(resp);
        },
        {drogon::Options}
    );

    // Add CORS headers to every response via a post-handling advice
    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr&, const drogon::HttpResponsePtr& resp) {
            resp->addHeader("Access-Control-Allow-Origin",  "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
        }
    );
}

} // namespace ipinv::app
