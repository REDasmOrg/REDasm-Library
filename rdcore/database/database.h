#pragma once

#include <string>
#include <rdapi/database/database.h>
#include "../libs/json/json.hpp"
#include "../object.h"

class Database: public Object
{
    public:
        typedef std::vector<u8> DatabaseData;
        typedef DatabaseData DecompiledData;
        typedef DatabaseData CompiledData;

    private:
        Database(const std::string& dbpath, const nlohmann::json& db);
        void writeValue(const nlohmann::json& value, RDDatabaseValue* dbvalue) const;

    public:
        bool query(std::string q, RDDatabaseValue* dbvalue) const;
        const std::string& name() const;

    public:
        static bool compileFile(const std::string& filepath, CompiledData& compiled);
        static bool decompileFile(const std::string& filepath, DecompiledData& decompiled);
        static bool compile(const DecompiledData& decompiled, CompiledData& compiled);
        static bool decompile(const CompiledData& compiled, DecompiledData& decompiled);
        static Database* create(const std::string& dbpath);
        static Database* open(const std::string& dbpath);

    private:
        static void read(const std::string& filepath, DatabaseData& data);
        static bool parseDecompiledFile(const std::string& filepath, nlohmann::json& j);
        static bool parseCompiledFile(const std::string& filepath, nlohmann::json& j);
        static bool parseDecompiled(const DecompiledData& decompiled, nlohmann::json& j);
        static bool parseCompiled(const CompiledData& compiled, nlohmann::json& j);
        static std::string locate(std::string dbname);
        static std::string dbpath(const std::string& dbname);

    private:
        mutable std::unordered_map<nlohmann::json::value_t, std::string> m_valuecache;
        std::string m_dbpath, m_dbname;
        nlohmann::json m_db;
};
