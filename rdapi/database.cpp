#include "database.h"
#include <rdcore/database.h>

RDDatabase* RDDatabase_Open(const char* dbname)
{
    if(!Database::exists(dbname)) return nullptr;
    return CPTR(RDDatabase, new Database(dbname));
}

bool RDDatabase_Select(RDDatabase* db, const char* obj) { return CPTR(Database, db)->select(obj); }
bool RDDatabase_Find(const RDDatabase* db, const char* key, RDDatabaseItem* item) { return CPTR(const Database, db)->find(key, item); }
