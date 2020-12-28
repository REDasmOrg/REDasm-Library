#include "database.h"
#include "../support/compression.h"
#include "../support/utils.h"
#include "../config.h"
#include <algorithm>
#include <fstream>

#define DATABASE_NAME_FIELD "@name"

Database::Database(const tao::json::value& tree): Object(), m_tree(tree) { }
Database::Database(): Object() { Database::initializeTree(m_tree); }
Database::Database(Context* ctx): Object(ctx) { Database::initializeTree(m_tree); }
const std::string& Database::name() const { m_tree.at(DATABASE_NAME_FIELD).to(m_name); return m_name; }
void Database::setName(const std::string& name) { m_tree[DATABASE_NAME_FIELD] = name; }

bool Database::query(std::string q, RDDatabaseValue* dbvalue) const
{
    if(!this->checkPointer(q)) return false;

    auto* val = m_tree.find(tao::json::pointer(q));
    return val ? this->extract(*val, dbvalue) : false;
}

bool Database::write(const std::string& path, const std::string& val)
{
    tao::json::pointer p = this->checkTree(path);
    if(p.empty()) return false;

    m_tree[p] = val;
    return true;
}

bool Database::write(const std::string& path, const Type* type)
{
    tao::json::pointer p = this->checkTree(path);
    if(p.empty()) return false;

    m_tree[p] = type->toJson();
    return true;
}

bool Database::add(const std::string& dbpath, const std::string& filepath)
{
    auto dbp = std::make_pair(dbpath, filepath);
    if(m_importeddb.count(dbp)) return true; // This db is already imported
    m_importeddb.insert(dbp);

    if(this->pathExists(dbpath)) return false;

    auto dbloc = Database::locate(filepath);
    if(dbloc.empty()) return false;

    tao::json::pointer p = this->checkTree(dbpath);
    if(p.empty()) return false;

    tao::json::value tree;
    if(!Database::parseFile(dbloc, tree)) return false;
    if(!Database::validateTree(tree)) return false;

    m_tree[p] = tree;
    return true;
}

bool Database::compile(const std::string& filepath) const
{
    if(!filepath.empty()) return false;

    auto res = tao::json::msgpack::to_string(m_tree);
    if(res.empty()) return false;

    Data data(res.begin(), res.end()), outdata;
    if(!Compression::compress(data, outdata) || outdata.empty()) return false;

    std::ofstream ofs(filepath, std::ios::binary);
    if(!ofs.is_open()) return false;
    ofs.write(reinterpret_cast<char*>(outdata.data()), outdata.size());
    return true;
}

const std::string& Database::decompile() const
{
    m_decompiled = tao::json::to_string(m_tree);
    return m_decompiled;
}

void Database::initializeTree(tao::json::value& tree)
{
    tree = {
        { DATABASE_NAME_FIELD, "" }
    };
}

bool Database::validateTree(const tao::json::value& tree)
{
    if(!tree.is_object()) return false;

    auto* name = tree.find(DATABASE_NAME_FIELD);
    return name && name->is_string();
}

Database* Database::open(const std::string& dbname)
{
    auto dbloc = Database::locate(dbname);
    if(dbloc.empty()) return nullptr;

    tao::json::value tree;
    if(!Database::parseFile(dbloc, tree)) return nullptr;
    if(!Database::validateTree(tree)) return nullptr;
    return new Database(tree);
}

bool Database::compileFile(const fs::path& filepath, Database::Data& outdata)
{
    tao::json::value v;
    if(!Database::parseDecompiledFile(filepath, v)) return false;

    auto res = tao::json::msgpack::to_string(v);
    if(res.empty()) return false;

    Data data(res.begin(), res.end());
    if(!Compression::compress(data, outdata)) return false;
    return !outdata.empty();
}

bool Database::decompileFile(const fs::path& filepath, Database::Data& outdata)
{
    tao::json::value tree;
    if(!Database::parseCompiledFile(filepath, tree)) return false;

    auto res = tao::json::msgpack::to_string(tree);
    if(res.empty()) return false;

    outdata = Data(res.begin(), res.end());
    return !outdata.empty();
}

void Database::extractObject(const tao::json::value& obj, RDDatabaseValue* outval) const
{
    Type* t = Type::load(obj);

    if(t)
    {
        m_typecache.reset(t); // Take ownership
        outval->type = DatabaseValueType_Type;
        outval->t = CPTR(RDType, t);
        return;
    }

    auto it = m_valuecache.insert_or_assign(obj.type(), tao::json::to_string(obj));
    outval->type = DatabaseValueType_Object;
    outval->obj = it.first->second.c_str();
}

bool Database::extract(const tao::json::value& inval, RDDatabaseValue* outval) const
{
    switch(inval.type())
    {
        case tao::json::type::STRING: {
            auto it = m_valuecache.insert_or_assign(inval.type(), inval.as<std::string>());
            outval->type = DatabaseValueType_String;
            outval->s = it.first->second.c_str();
            return true;
        }

        case tao::json::type::DOUBLE:
            outval->type = DatabaseValueType_Float;
            inval.to(outval->f);
            return true;

        case tao::json::type::SIGNED:
            outval->type = DatabaseValueType_Int;
            inval.to(outval->i);
            return true;

        case tao::json::type::UNSIGNED:
            outval->type = DatabaseValueType_UInt;
            inval.to(outval->u);
            return true;

        case tao::json::type::BOOLEAN:
            outval->type = DatabaseValueType_Bool;
            inval.to(outval->b);
            return true;

        case tao::json::type::OBJECT:
            this->extractObject(inval, outval);
            return true;

        default: break;
    }

    return false;
}

bool Database::checkPointer(std::string& path) const
{
    if(path.front() != '/') path = "/" + path;
    if(!path.find("/@")) return false; // Disallow database field access
    if(path == "/") path.clear();
    return true;
}

bool Database::pathExists(std::string path) const
{
    this->checkPointer(path);
    return m_tree.find(path);
}

tao::json::pointer Database::checkTree(std::string path)
{
    if(!this->checkPointer(path)) return { };

    tao::json::pointer ptr(path);
    auto* obj = &m_tree;

    for(const auto& item : ptr)
    {
        auto* itemobj = obj->find(item.key());

        if(!itemobj)
        {
            (*obj)[item.key()] = tao::json::empty_object;
            obj = std::addressof((*obj)[item.key()]);
        }
        else
            obj = itemobj;
    }

    return ptr;
}

bool Database::parseDecompiledFile(const fs::path& filepath, tao::json::value& j)
{
    try {
        j = tao::json::from_file(filepath);
    }
    catch(tao::json::pegtl::parse_error& e) {
        rd_cfg->log(e.what());
        return false;
    }

    return true;
}

bool Database::parseCompiledFile(const fs::path& filepath, tao::json::value& j)
{
    try {
        Data data;
        if(!Compression::decompressFile(filepath.string(), data)) return false;

        std::string mp(data.begin(), data.end());
        j = tao::json::msgpack::from_string(mp);
    }
    catch(tao::json::pegtl::parse_error& e) {
        rd_cfg->log(e.what());
        return false;
    }

    return true;
}

bool Database::parseFile(const fs::path& filepath, tao::json::value& j)
{
    std::string ext = filepath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if(ext == DATABASE_JSON_EXT) return Database::parseDecompiledFile(filepath, j);
    else if(ext == DATABASE_RDB_EXT) return Database::parseCompiledFile(filepath, j);
    else
    {
        rd_cfg->log("Unknown file type: " + Utils::quoted(ext));
        return false;
    }

    return true;
}

fs::path Database::locatePath(const fs::path& dbpath)
{
    fs::directory_entry dbentry;

    for(const auto& searchpath : rd_cfg->databasePaths())
    {
        dbentry.assign(searchpath / dbpath);
        if(dbentry.is_regular_file()) return dbentry.path();

        dbentry.assign(searchpath / DATABASE_FOLDER_NAME / dbpath);
        if(dbentry.is_regular_file()) return dbentry.path();
    }

    return { };
}

fs::path Database::locateAs(fs::path dbpath, const platform_string& ext)
{
    dbpath.replace_extension(ext);
    if(fs::exists(dbpath)) return dbpath;
    return Database::locatePath(dbpath);
}

fs::path Database::locate(fs::path dbpath)
{
    if(dbpath.extension().empty())
    {
        static const std::vector<platform_string> ALLOWED_EXT = {
            DATABASE_RDB_EXT, DATABASE_JSON_EXT
        };

        for(const auto& ext : ALLOWED_EXT)
        {
            auto p = Database::locateAs(dbpath, ext);
            if(!p.empty()) return p;
        }
    }

    return Database::locatePath(dbpath);
}
