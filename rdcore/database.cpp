#include "database.h"
#include "context.h"
#include <fstream>

#define DATABASE_FOLDER_NAME "database"
#define NS_SUFFIX            ".json"

Database::Database(const std::string& dbname) { m_db.assign(this->find(dbname)); }

bool Database::select(const std::string& obj)
{
    auto it = m_objects.find(obj);

    if(it != m_objects.end())
    {
        m_currobj = it->second;
        return true;
    }

    std::filesystem::directory_entry entry(std::filesystem::path(m_db).append(obj + NS_SUFFIX));
    if(!entry.is_regular_file()) return false;

    std::ifstream ifs(entry.path(), std::ios::binary);
    if(!ifs.is_open()) return false;

    try {
        nlohmann::json data = nlohmann::json::parse(ifs);
        if(!data.is_object()) return false;

        m_objects[obj] = data;
        m_currobj = m_objects[obj];
    } catch(nlohmann::json::parse_error) {
        return false;
    }

    return true;
}

bool Database::find(const std::string& key, RDDatabaseItem* item) const
{
    if(!m_currobj) return false;

    auto it = m_currobj->find(key);
    if(it == m_currobj->end()) return false;
    return item ? this->writeItem(*it, item) : true;
}

bool Database::exists(const std::string& dbname) { return !Database::find(dbname).empty(); }

bool Database::writeItem(const nlohmann::json& value, RDDatabaseItem* item) const
{
    switch(value.type())
    {
        case nlohmann::detail::value_t::string:
            item->type = DatabaseItemType_String;
            item->s_value = value.get_ptr<const nlohmann::json::string_t*>()->c_str();
            break;

        case nlohmann::detail::value_t::number_float:
            item->type = DatabaseItemType_Bool;
            item->f_value = value;
            break;

        case nlohmann::detail::value_t::number_integer:
            item->type = DatabaseItemType_Int;
            item->i_value = value;
            break;

        case nlohmann::detail::value_t::number_unsigned:
            item->type = DatabaseItemType_UInt;
            item->u_value = value;
            break;

        case nlohmann::detail::value_t::boolean:
            item->type = DatabaseItemType_Bool;
            item->b_value = value;
            break;

        default: return false;
    }

    return true;
}

std::string Database::find(const std::string& dbname)
{
    // Try with runtime path
    std::filesystem::directory_entry dbentry(std::filesystem::path(rd_ctx->runtimePath()).append(dbname).make_preferred());
    if(dbentry.is_directory()) return dbentry.path();

    // Try with runtime path + "database"
    dbentry.assign(std::filesystem::path(rd_ctx->runtimePath()).append(DATABASE_FOLDER_NAME).append(dbname).make_preferred());
    if(dbentry.is_directory()) return dbentry.path();

    // Search everywhere
    for(const std::string& searchpath : rd_ctx->databasePaths())
    {
        dbentry.assign(std::filesystem::path(searchpath).append(dbname).make_preferred());
        if(dbentry.is_directory()) return dbentry.path();

        dbentry.assign(std::filesystem::path(searchpath).append(DATABASE_FOLDER_NAME).append(dbname).make_preferred());
        if(dbentry.is_directory()) return dbentry.path();
    }

    return std::string();
}
