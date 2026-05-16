// ============================================================
// src/controllers/ServiceIdQueryController.cpp
// GET /ip-inventory/serviceId?serviceId=xxx
// Returns all assigned IPs for a given serviceId.
// ============================================================
#include "controllers/ServiceIdQueryController.h"
#include "services/ServiceIdQueryService.h"
#include "common/Types.h"

namespace svc = ipinv::services;
using ipinv::errorResponse;

namespace ipinv::controllers {

void getServiceId(const Req& req, Cb&& cb)
{
    std::string serviceId = req->getParameter("serviceId");
    if (serviceId.empty()) {
        cb(errorResponse(drogon::k400BadRequest, "Query parameter 'serviceId' is required")); return;
    }
    svc::getByServiceId(serviceId, std::move(cb));
}

} // namespace ipinv::controllers
