#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include <rdapi/database.h>
#include "libs/json/json.hpp"
#include "object.h"

class Database: public Object
{
    public:
        Database(const std::string& dbname);
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
