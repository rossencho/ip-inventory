// ============================================================
// src/utils/JsonValidator.cpp
// ============================================================
#include "utils/JsonValidator.h"
#include <set>

namespace ipinv::utils {

std::optional<std::string> requireString(const Json::Value& v,
                                          const std::string& field) {
    if (!v.isMember(field))
        return "Missing field: " + field;
    if (!v[field].isString())
        return "Field '" + field + "' must be a string";
    if (v[field].asString().empty())
        return "Field '" + field + "' must not be empty";
    return std::nullopt;
}

std::optional<std::string> requireArray(const Json::Value& v,
                                         const std::string& field) {
    if (!v.isMember(field))
        return "Missing field: " + field;
    if (!v[field].isArray())
        return "Field '" + field + "' must be an array";
    return std::nullopt;
}

bool isValidIpType(const std::string& t) {
    static const std::set<std::string> valid{"IPv4", "IPv6", "Both"};
    return valid.count(t) > 0;
}

} // namespace ipinv::utils
