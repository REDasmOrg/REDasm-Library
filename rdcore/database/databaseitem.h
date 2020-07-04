#pragma once

#include "../libs/json/json.hpp"
#include "../object.h"
#include <rdapi/database/database.h>
#include <unordered_map>
#include <memory>
#include <string>

class Database;

class DatabaseItem: public Object
{
    private:
        typedef std::unique_ptr<DatabaseItem> DatabaseItemPtr;

    public:
        DatabaseItem(const std::string& name, rd_type type, DatabaseItem* parent, Database* db);
        Database* database() const;
        DatabaseItem* parent() const;
        DatabaseItem* set(const std::string& name, rd_type type);
        DatabaseItem* get(const std::string& name) const;
        void erase(const std::string& name);

    protected:
        DatabaseItem(Database* db);
        nlohmann::json serialize() const;

    private:
        std::unordered_map<std::string, DatabaseItemPtr> m_items;
        std::string m_name;
        rd_type m_type{DatabaseItemType_None};
        DatabaseItem* m_parent{nullptr};
        Database* m_database{nullptr};
};
