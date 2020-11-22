#include "database.h"
#include "../config.h"
#include "../support/utils.h"
#include "../support/compression.h"
#include <fstream>

Database::Database(const fs::path& dbpath, const nlohmann::json& db): Object(), m_dbfilepath(dbpath), m_dbname(dbpath.stem().string()), m_db(db) { }

bool Database::compileFile(const fs::path &filepath, CompiledData& compiled)
{
    DecompiledData decompiled;
    Database::read(filepath, decompiled);
    return Database::compile(decompiled, compiled);
}

bool Database::decompileFile(const fs::path &filepath, DecompiledData& decompiled)
{
    CompiledData compiled;
    Database::read(filepath, compiled);
    return Database::decompile(compiled, decompiled);
}

bool Database::compile(const DecompiledData& decompiled, Database::CompiledData& compiled)
{
    nlohmann::json compiledb;
    if(!Database::parseDecompiled(decompiled, compiledb)) return false;
    auto tempdata = nlohmann::json::to_msgpack(compiledb);
    if(!Compression::compress(tempdata, compiled)) return false;
    return !compiled.empty();
}

bool Database::decompile(const CompiledData& compiled, Database::DecompiledData& decompiled)
{
    nlohmann::json decompiledb;
    if(!Database::parseCompiled(compiled, decompiledb)) return false;
    auto d = decompiledb.dump(2);
    decompiled = DecompiledData(d.begin(), d.end());
    return !decompiled.empty();
}

const std::string& Database::name() const { return m_dbname; }

void Database::writeValue(const nlohmann::json& value, RDDatabaseValue* dbvalue) const
{
    switch(value.type())
    {
        case nlohmann::detail::value_t::string:
            dbvalue->type = DatabaseValueType_String;
            dbvalue->s = value.get_ptr<const nlohmann::json::string_t*>()->c_str();
            break;

        case nlohmann::detail::value_t::number_float:
            dbvalue->type = DatabaseValueType_Float;
            dbvalue->f = value;
            break;

        case nlohmann::detail::value_t::number_integer:
            dbvalue->type = DatabaseValueType_Int;
            dbvalue->i = value;
            break;

        case nlohmann::detail::value_t::number_unsigned:
            dbvalue->type = DatabaseValueType_UInt;
            dbvalue->u = value;
            break;

        case nlohmann::detail::value_t::boolean:
            dbvalue->type = DatabaseValueType_Bool;
            dbvalue->b = value;
            break;

        case nlohmann::detail::value_t::array:
        {
            auto it = m_valuecache.insert_or_assign(value.type(), value.dump());
            dbvalue->type = DatabaseValueType_Array;
            dbvalue->arr = it.first->second.c_str();
            break;
        }

        case nlohmann::detail::value_t::object:
        {
            auto it = m_valuecache.insert_or_assign(value.type(), value.dump());
            dbvalue->type = DatabaseValueType_Object;
            dbvalue->obj = it.first->second.c_str();
            break;
        }

        default:
            REDasmError("Unhandled Type: " + std::string(value.type_name()));
            break;
    }
}

bool Database::query(std::string q, RDDatabaseValue* dbvalue) const
{
    if(q.empty()) return false;
    if(q[0] != '/') q = "/" + q;

    try {
        nlohmann::json value;

        if(q != "/")
        {
            nlohmann::json::json_pointer p(q);
            if(!m_db.contains(p)) return false;
            value = m_db[p];
        }
        else
            value = m_db;

        if(value.is_null()) return false;
        if(dbvalue) Database::writeValue(value, dbvalue);
    }
    catch(nlohmann::json::parse_error& e) {
        rd_cfg->log(e.what());
        return false;
    }

    return true;
}

std::string Database::filePath() const { return m_dbfilepath.string(); }
Database* Database::create(const fs::path& dbpath) { return new Database(dbpath, nlohmann::json::object()); }

Database* Database::open(const fs::path& dbpath)
{
    auto path = Database::locate(dbpath);
    if(path.empty()) return nullptr;

    nlohmann::json db;
    if(!Database::parseCompiledFile(path, db)) return nullptr;
    return new Database(path, db);
}

void Database::read(const fs::path& filepath, Database::DatabaseData& data)
{
    std::ifstream stream(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    if(!stream.is_open()) return;

    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    data.resize(size);
    stream.read(reinterpret_cast<char*>(data.data()), size);
}

bool Database::parseDecompiledFile(const fs::path& filepath, nlohmann::json& j)
{
    DecompiledData decompiled;
    Database::read(filepath, decompiled);
    return Database::parseDecompiled(decompiled, j);
}

bool Database::parseCompiledFile(const fs::path& filepath, nlohmann::json& j)
{
    CompiledData compiled;
    Database::read(filepath, compiled);
    return Database::parseCompiled(compiled, j);
}

bool Database::parseDecompiled(const Database::DecompiledData& decompiled, nlohmann::json& j)
{
    try {
        j = nlohmann::json::parse(decompiled);
    }  catch (nlohmann::json::parse_error& e) {
        rd_cfg->log(e.what());
        return false;
    }

    return true;
}

bool Database::parseCompiled(const CompiledData& compiled, nlohmann::json& j)
{
    try {
        CompiledData tempdata;
        Compression::decompress(compiled, tempdata);
        j = nlohmann::json::from_msgpack(tempdata);
    }  catch (nlohmann::json::parse_error& e) {
        rd_cfg->log(e.what());
        return false;
    }

    return true;
}

fs::path Database::locate(fs::path dbname)
{
    if(dbname.extension() != DATABASE_RDB_EXT) dbname.replace_extension(DATABASE_RDB_EXT);
    if(fs::exists(dbname)) return dbname;

    fs::directory_entry dbentry;

    // Search everywhere
    for(const auto& searchpath : rd_cfg->databasePaths())
    {
        dbentry.assign(searchpath / dbname);
        if(dbentry.is_regular_file()) return dbentry.path();

        dbentry.assign(searchpath / DATABASE_FOLDER_NAME / dbname);
        if(dbentry.is_regular_file()) return dbentry.path();
    }

    return { };
}
