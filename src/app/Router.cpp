// ============================================================
// src/app/Router.cpp
// Maps URL paths to controller functions.
// ============================================================
#include "app/Router.h"
#include "controllers/IpPoolController.h"
#include "controllers/ReserveIpController.h"
#include "controllers/AssignIpController.h"
#include "controllers/TerminateIpController.h"
#include "controllers/ServiceIdChangeController.h"
#include "controllers/ServiceIdQueryController.h"
#include <drogon/drogon.h>

namespace ipinv::app {

void registerRoutes() {
    drogon::app().registerHandler(
        "/ip-inventory/ip-pool",
        &controllers::ipPool, {drogon::Post});
    drogon::app().registerHandler(
        "/ip-inventory/reserve-ip",
        &controllers::reserveIp, {drogon::Post});
    drogon::app().registerHandler(
        "/ip-inventory/assign-ip-serviceId",
        &controllers::assignIp, {drogon::Post});
    drogon::app().registerHandler(
        "/ip-inventory/terminate-ip-serviceId",
        &controllers::terminateIp, {drogon::Post});
    drogon::app().registerHandler(
        "/ip-inventory/serviceId-change",
        &controllers::serviceChange, {drogon::Post});
    drogon::app().registerHandler(
        "/ip-inventory/serviceId",
        &controllers::getServiceId, {drogon::Get});
}

} // namespace ipinv::app
