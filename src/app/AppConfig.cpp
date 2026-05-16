// ============================================================
// src/app/AppConfig.cpp
// DB client initialisation and expiry job startup.
// ============================================================
#include "app/AppConfig.h"
#include "repository/IpRepository.h"
#include "services/ReservationExpiryJob.h"

#include <drogon/drogon.h>
#include <json/json.h>
#include <fstream>
#include <filesystem>

// Persistent client — must outlive app().run()
static drogon::orm::DbClientPtr s_dbClient;

namespace ipinv::app
{

    std::string resolveConfigPath(const std::string &relative)
    {
        std::error_code ec;
        auto exe = std::filesystem::read_symlink("/proc/self/exe", ec);
        if (!ec)
        {
            auto c = exe.parent_path() / relative;
            if (std::filesystem::exists(c))
                return c.string();
            c = exe.parent_path().parent_path() / relative;
            if (std::filesystem::exists(c))
                return c.string();
        }
        return relative;
    }

    void initDbClient(const std::string &configPath)
    {
        std::string host = "127.0.0.1";
        int port = 5432;
        std::string dbname = "data";
        std::string user = "user_placeholder";
        std::string passwd = "passwd_placeholder";
        int connNum = 10;

        try
        {
            std::ifstream f(configPath);
            Json::Value root;
            Json::CharReaderBuilder rb;
            std::string errs;
            if (Json::parseFromStream(rb, f, &root, &errs) && root.isMember("db_clients") && root["db_clients"].isArray() && !root["db_clients"].empty())
            {
                const auto &db = root["db_clients"][0];
                if (db.isMember("host"))
                    host = db["host"].asString();
                if (db.isMember("port"))
                    port = db["port"].asInt();
                if (db.isMember("dbname"))
                    dbname = db["dbname"].asString();
                if (db.isMember("user"))
                    user = db["user"].asString();
                if (db.isMember("passwd"))
                    passwd = db["passwd"].asString();
                if (db.isMember("connection_number"))
                    connNum = db["connection_number"].asInt();
            }
        }
        catch (...)
        {
            LOG_WARN << "[AppConfig] Could not parse db_clients from config — using defaults";
        }

        const std::string connStr =
            "host=" + host +
            " port=" + std::to_string(port) +
            " dbname=" + dbname +
            " user=" + user +
            " password=" + passwd;

        s_dbClient = drogon::orm::DbClient::newPgClient(connStr, connNum);
        ipinv::repository::globalClient() = s_dbClient;
        LOG_INFO << "[AppConfig] PostgreSQL client created ("
                 << connNum << " connections) -> " << host << ":" << port << "/" << dbname;
    }

    void startExpiryJob()
    {
        int interval = 30, timeout = 300;

        const auto &cfg = drogon::app().getCustomConfig();
        if (cfg.isObject() && cfg.isMember("ip_inventory"))
        {
            const auto &ip = cfg["ip_inventory"];
            if (ip.isObject())
            {
                if (ip.isMember("expiry_check_interval_seconds"))
                    interval = ip["expiry_check_interval_seconds"].asInt();
                if (ip.isMember("reservation_timeout_seconds"))
                    timeout = ip["reservation_timeout_seconds"].asInt();
            }
        }

        ipinv::services::ReservationExpiryJob::start(interval, timeout);
        LOG_INFO << "[AppConfig] Expiry job started (interval=" << interval
                 << "s, timeout=" << timeout << "s)";
    }

} // namespace ipinv::app
