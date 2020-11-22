#pragma once

#include <string>
#include <rdapi/database/database.h>
#include "../libs/json/json.hpp"
#include "../config.h"
#include "../object.h"

class Database: public Object
{
    public:
        typedef std::vector<u8> DatabaseData;
        typedef DatabaseData DecompiledData;
        typedef DatabaseData CompiledData;

    private:
        Database(const fs::path& dbpath, const nlohmann::json& db);
        void writeValue(const nlohmann::json& value, RDDatabaseValue* dbvalue) const;

    public:
        bool query(std::string q, RDDatabaseValue* dbvalue) const;
        std::string filePath() const;
        const std::string& name() const;

    public:
        static bool compileFile(const fs::path& filepath, CompiledData& compiled);
        static bool decompileFile(const fs::path& filepath, DecompiledData& decompiled);
        static bool compile(const DecompiledData& decompiled, CompiledData& compiled);
        static bool decompile(const CompiledData& compiled, DecompiledData& decompiled);
        static Database* create(const fs::path& dbpath);
        static Database* open(const fs::path &dbpath);

    private:
        static void read(const fs::path &filepath, DatabaseData& data);
        static bool parseDecompiledFile(const fs::path &filepath, nlohmann::json& j);
        static bool parseCompiledFile(const fs::path &filepath, nlohmann::json& j);
        static bool parseDecompiled(const DecompiledData& decompiled, nlohmann::json& j);
        static bool parseCompiled(const CompiledData& compiled, nlohmann::json& j);
        static fs::path locate(fs::path dbname);
        static std::string dbpath(const std::string& dbname);

    private:
        mutable std::unordered_map<nlohmann::json::value_t, std::string> m_valuecache;
        fs::path m_dbfilepath;
        std::string m_dbname;
        nlohmann::json m_db;
};
