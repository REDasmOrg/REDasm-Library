#include "lmdb.h"
#include "lmdbexception.h"
#include <string>

#define LMDB_MAP_SIZE  4UL * 1024UL * 1024UL * 1024UL // 4GiB

LMDB::~LMDB() { this->close(); }
LMDB::operator bool() const { return m_env; }

void LMDB::open(const std::string& filepath, unsigned int dbflags, unsigned int flags)
{
    if(m_env) return;

    m_dbflags = dbflags;
    lmdb_check(mdb_env_create(&m_env));
    lmdb_check(mdb_env_set_mapsize(m_env, LMDB_MAP_SIZE));
    lmdb_check(mdb_env_open(m_env, filepath.c_str(), flags | MDB_NOSUBDIR, 0664));
}

LMDBTransactionPtr LMDB::transaction() const { return LMDBTransactionPtr(new LMDBTransaction(m_env, m_dbflags)); }

void LMDB::close()
{
    if(!m_env) return;

    mdb_dbi_close(m_env, m_dbi);
    mdb_env_close(m_env);
    m_env = nullptr;
}
