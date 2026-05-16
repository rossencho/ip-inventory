#pragma once
// ============================================================
// include/utils/JsonValidator.h
// ============================================================
#include <json/json.h>
#include <string>
#include <optional>

namespace ipinv::utils {

// Returns error string if field is missing/wrong type, else nullopt
std::optional<std::string> requireString(const Json::Value& v,
                                          const std::string& field);

std::optional<std::string> requireArray(const Json::Value& v,
                                         const std::string& field);

bool isValidIpType(const std::string& t);

} // namespace ipinv::utils
