#pragma once
// ============================================================
// include/app/AppConfig.h
// ============================================================
#include <string>

namespace ipinv::app {

// Resolves path relative to the running binary's directory
std::string resolveConfigPath(const std::string& relative);

// Reads db_clients[0] from configPath, creates newPgClient(),
// stores it in db::globalClient(). Call after loadConfigFile().
void initDbClient(const std::string& configPath);

// Reads custom_config.ip_inventory and starts ReservationExpiryJob.
// Call inside registerBeginningAdvice (live event loop).
void startExpiryJob();

} // namespace ipinv::app
