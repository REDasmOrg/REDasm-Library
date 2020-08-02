#include "databaseitem.h"

#define DATABASE_TYPE_FIELD  "@type"

DatabaseItem::DatabaseItem(Database* db): m_database(db) { }
DatabaseItem::DatabaseItem(const std::string& name, rd_type type, DatabaseItem* parent, Database* db): Object(), m_name(name), m_type(type), m_parent(parent), m_database(db) { }
Database* DatabaseItem::database() const { return m_database; }

DatabaseItem* DatabaseItem::set(const std::string& name, rd_type type)
{
    auto it = m_children.emplace(name, new DatabaseItem(name, type, this, m_database));
    return it.first->second.get();
}

DatabaseItem* DatabaseItem::get(const std::string& name) const
{
    auto it = m_children.find(name);
    return it != m_children.end() ? it->second.get() : nullptr;
}

void DatabaseItem::erase(const std::string& name) { m_children.erase(name); }

nlohmann::json DatabaseItem::serialize() const
{
    nlohmann::json obj = nlohmann::json::object();
    obj[DATABASE_TYPE_FIELD] = m_type;

    for(const auto& [name, item] : m_children)
    {
        switch(name.index())
        {
            case 0: obj[std::get<0>(name)] = item->serialize(); break;
            case 1: obj[std::get<1>(name)] = item->serialize(); break;
            default: break;
        }
    }

    return obj;
}

DatabaseItem* DatabaseItem::parent() const { return m_parent; }
