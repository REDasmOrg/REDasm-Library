#include "databaseitem.h"

DatabaseItem::DatabaseItem(Database* db): m_database(db) { }
DatabaseItem::DatabaseItem(const std::string& name, rd_type type, DatabaseItem* parent, Database* db): Object(), m_name(name), m_type(type), m_parent(parent), m_database(db) { }
Database* DatabaseItem::database() const { return m_database; }

DatabaseItem* DatabaseItem::set(const std::string& name, rd_type type)
{
    auto it = m_items.emplace(name, new DatabaseItem(name, type, this, m_database));
    return it.first->second.get();
}

DatabaseItem* DatabaseItem::get(const std::string& name) const
{
    auto it = m_items.find(name);
    return it != m_items.end() ? it->second.get() : nullptr;
}

void DatabaseItem::erase(const std::string& name) { m_items.erase(name); }

nlohmann::json DatabaseItem::serialize() const
{
    nlohmann::json obj = nlohmann::json::object();

    for(const auto& [name, item] : m_items)
        obj[name] = item->serialize();

    return obj;
}

DatabaseItem* DatabaseItem::parent() const { return m_parent; }
