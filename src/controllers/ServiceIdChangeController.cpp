// ============================================================
// src/controllers/ServiceIdChangeController.cpp
// POST /ip-inventory/serviceId-change
// Re-assigns all IPs from one serviceId to another.
// ============================================================
#include "controllers/ServiceIdChangeController.h"
#include "services/ServiceIdChangeService.h"
#include "utils/JsonValidator.h"
#include "common/Types.h"

namespace iu  = ipinv::utils;
namespace svc = ipinv::services;
using ipinv::errorResponse;

namespace ipinv::controllers {

void serviceChange(const Req& req, Cb&& cb)
{
    auto body = req->getJsonObject();
    if (!body) { cb(errorResponse(drogon::k400BadRequest, "Invalid or missing JSON body")); return; }
    if (auto e = iu::requireString(*body, "serviceIdOld"); e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }
    if (auto e = iu::requireString(*body, "serviceId");    e) { cb(errorResponse(drogon::k400BadRequest, *e)); return; }

    std::string oldId = (*body)["serviceIdOld"].asString();
    std::string newId = (*body)["serviceId"].asString();
    if (oldId == newId) { cb(errorResponse(drogon::k400BadRequest, "serviceIdOld and serviceId must differ")); return; }
    svc::changeServiceId(oldId, newId, std::move(cb));
}

} // namespace ipinv::controllers
