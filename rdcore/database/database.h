#pragma once

#include <rdapi/database/database.h>
#include <filesystem>
#include "../libs/tao/json.hpp"
#include "../types/definitions.h"
#include "../object.h"

namespace fs = std::filesystem;

class Database: public Object
{
    public:
        typedef std::vector<u8> Data;

    private:
        Database(const tao::json::value& tree);

    public:
        Database();
        Database(Context* ctx);
        const std::string& name() const;
        void setName(const std::string& name);
        bool query(std::string q, RDDatabaseValue* dbvalue) const;
        bool write(const std::string& path, const std::string& val);
        bool write(const std::string& path, const Type* type);
        bool add(const std::string& path, const std::string& dbpath);
        bool compile(const std::string& filepath) const;
        const std::string& decompile() const;

    public:
        static void initializeTree(tao::json::value& tree);
        static bool validateTree(const tao::json::value& tree);
        static Database* open(const std::string& dbname);
        static bool compileFile(const fs::path& filepath, Data& outdata);
        static bool decompileFile(const fs::path& filepath, Data& outdata);

    private:
        bool extract(const tao::json::value& inval, RDDatabaseValue* outval) const;
        bool checkPointer(std::string& path) const;
        tao::json::pointer checkTree(std::string path);

    private:
        static bool parseDecompiledFile(const fs::path &filepath, tao::json::value& j);
        static bool parseCompiledFile(const fs::path &filepath, tao::json::value& j);
        static fs::path locate(fs::path dbname);

    private:
        mutable std::unordered_map<tao::json::type, std::string> m_valuecache;
        mutable std::string m_decompiled, m_name;
        tao::json::value m_tree;
};
