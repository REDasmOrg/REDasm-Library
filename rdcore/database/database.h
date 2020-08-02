#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include <rdapi/database/database.h>
#include "../libs/json/json.hpp"
#include "databaseitem.h"

class Database: public DatabaseItem
{
    public:
        Database(const std::string& dbname);
        bool save(const std::string& filepath) const;
        static Database* load(const std::string& dbname);

    private:
        std::string m_dbname;
        nlohmann::json m_database;

    public: // vvv OLD vvv
        bool select(const std::string& obj);
        bool find(const std::string& key, RDDatabaseItem* item) const;
        static bool exists(const std::string& dbname);

    private:
        bool writeItem(const nlohmann::json& value, RDDatabaseItem* item) const;
        static std::string find(const std::string& dbname);
        static std::string dbpath(const std::string& dbname);

    private:
        std::optional<nlohmann::json> m_currobj;
        std::unordered_map<std::string, nlohmann::json> m_objects;
        std::filesystem::path m_db;
};
