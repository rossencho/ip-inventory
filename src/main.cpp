// ============================================================
// src/main.cpp
// ============================================================
#include <drogon/drogon.h>
#include "app/AppConfig.h"
#include "app/Router.h"
#include "app/CorsMiddleware.h"

int main(int argc, char* argv[]) {
    const std::string configPath = (argc > 1)
        ? argv[1]
        : ipinv::app::resolveConfigPath("config/config.json");

    drogon::app().loadConfigFile(configPath);
    ipinv::app::initDbClient(configPath);
    ipinv::app::registerCors();
    ipinv::app::registerRoutes();
    drogon::app().registerBeginningAdvice(ipinv::app::startExpiryJob);
    drogon::app().run();
    return 0;
}
