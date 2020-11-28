#include "database.h"
#include <rdcore/database/database.h>

RDDatabase* RDDatabase_Open(const char* dbname)
{
    if(!dbname) return nullptr;
    return CPTR(RDDatabase, Database::open(dbname));
}

RDDatabase* RDDatabase_Create(const char* dbname)
{
    if(!dbname) return nullptr;
    return CPTR(RDDatabase, Database::create(dbname));
}

size_t RDDatabase_CompileFile(const char* filepath, const u8** compiled)
{
    static Database::CompiledData data;

    if(!filepath) return 0;
    if(!Database::compileFile(filepath, data)) return 0;
    *compiled = data.data();
    return data.size();
}

size_t RDDatabase_DecompileFile(const char* filepath, const u8** decompiled)
{
    static Database::DecompiledData data;

    if(!filepath) return 0;
    if(!Database::decompileFile(filepath, data)) return 0;
    *decompiled = data.data();
    return data.size();
}

const char* RDDatabase_GetName(const RDDatabase* db) { return CPTR(const Database, db)->name().c_str(); }

const char* RDDatabase_GetFilePath(const RDDatabase* db)
{
    static std::string filepath;
    filepath = CPTR(const Database, db)->filePath();
    return filepath.c_str();
}

bool RDDatabase_Query(const RDDatabase* db, const char* q, RDDatabaseValue* dbvalue) { return q ? CPTR(const Database, db)->query(q, dbvalue) : false; }
