#include "database.h"
#include <rdcore/database/database.h>
#include <rdcore/types/definitions.h>

RDDatabase* RDDatabase_Open(const char* dbname)
{
    if(!dbname) return nullptr;
    return CPTR(RDDatabase, Database::open(dbname));
}

RDDatabase* RDDatabase_Create() { return CPTR(RDDatabase, new Database()); }

size_t RDDatabase_CompileFile(const char* filepath, const u8** compiled)
{
    static Database::Data data;

    if(!filepath) return 0;
    if(!Database::compileFile(filepath, data)) return 0;
    *compiled = data.data();
    return data.size();
}

size_t RDDatabase_DecompileFile(const char* filepath, const u8** decompiled)
{
    static Database::Data data;

    if(!filepath) return 0;
    if(!Database::decompileFile(filepath, data)) return 0;
    *decompiled = data.data();
    return data.size();
}

void RDDatabase_SetName(RDDatabase* db, const char* name) { if(name) CPTR(Database, db)->setName(name); }
const char* RDDatabase_GetName(const RDDatabase* db) { return CPTR(const Database, db)->name().c_str(); }
const char* RDDatabase_Decompile(const RDDatabase* db) { return CPTR(const Database, db)->decompile().c_str(); }
bool RDDatabase_Query(const RDDatabase* db, const char* q, RDDatabaseValue* dbvalue) { return q ? CPTR(const Database, db)->query(q, dbvalue) : false; }

bool RDDatabase_WriteString(RDDatabase* db, const char* path, const char* s)
{
    if(!db || !path || !s) return false;
    return CPTR(Database, db)->write(path, s);
}

bool RDDatabase_WriteType(RDDatabase* db, const char* path, const RDType* t)
{
    if(!db || !path || !t) return false;
    return CPTR(Database, db)->write(path, CPTR(const Type, t));
}

bool RDDatabase_Add(RDDatabase* db, const char* path, const char* dbname)
{
    if(!db || !path || !dbname) return false;
    return CPTR(Database, db)->add(path, dbname);
}

bool RDDatabase_Compile(const RDDatabase* db, const char* filepath)
{
    if(!db || !filepath) return false;
    return CPTR(const Database, db)->compile(filepath);
}
