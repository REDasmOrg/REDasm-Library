#include "database.h"
#include <rdcore/database/database.h>

RDDatabase* RDDatabase_Open(const char* dbname)
{
    if(!Database::exists(dbname)) return nullptr;
    return CPTR(RDDatabase, new Database(dbname));
}

bool RDDatabase_Select(RDDatabase* db, const char* obj) { return CPTR(Database, db)->select(obj); }
bool RDDatabase_Find(const RDDatabase* db, const char* key, RDDatabaseItem* item) { return CPTR(const Database, db)->find(key, item); }

RDDatabase* RDDatabase_Create(const char* dbname) { return CPTR(RDDatabase, new Database(dbname)); }
RDDatabase* RDDatabase_Load(const char* dbname) { return CPTR(RDDatabase, Database::load(dbname)); }
bool RDDatabase_Save(const RDDatabase* db, const char* filepath) { return CPTR(const Database, db)->save(filepath); }
RDDatabaseItemNew* RDDatabase_Set(RDDatabase* db, const char* name, rd_type type) { return CPTR(RDDatabaseItemNew, CPTR(Database, db)->set(name, type)); }
RDDatabaseItemNew* RDDatabase_Get(const RDDatabase* db, const char* name) { return CPTR(RDDatabaseItemNew, CPTR(const Database, db)->get(name)); }

RDDatabaseItemNew* RDDatabaseItem_Set(RDDatabaseItemNew* dbitem, const char* name, rd_type type) { return CPTR(RDDatabaseItemNew, CPTR(DatabaseItem, dbitem)->set(name, type)); }
RDDatabaseItemNew* RDDatabaseItem_Get(const RDDatabaseItemNew* dbitem, const char* name) { return CPTR(RDDatabaseItemNew, CPTR(const DatabaseItem, dbitem)->get(name)); }
RDDatabaseItemNew* RDDatabaseItem_GetParent(const RDDatabaseItemNew* dbitem) { return CPTR(RDDatabaseItemNew, CPTR(const DatabaseItem, dbitem)->parent()); }
RDDatabase* RDDatabaseItem_GetDatabase(const RDDatabaseItemNew* dbitem) { return CPTR(RDDatabase, CPTR(const DatabaseItem, dbitem)->database()); }
