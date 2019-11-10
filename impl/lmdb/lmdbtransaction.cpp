#include "lmdbtransaction.h"
#include "lmdbexception.h"

namespace REDasm {

LMDBTransaction::LMDBTransaction(MDB_env* env, unsigned int dbflags): m_env(env)
{
    lmdb_check(mdb_txn_begin(env, nullptr, 0, &m_txn));
    lmdb_check(mdb_dbi_open(m_txn, nullptr, dbflags, &m_dbi));
}

LMDBTransaction::~LMDBTransaction()
{
    if(!m_txn) return;
    mdb_dbi_close(m_env, m_dbi);
}

void LMDBTransaction::commit() { mdb_txn_commit(m_txn); }
void LMDBTransaction::abort() { mdb_txn_abort(m_txn); }

void LMDBTransaction::puts(const String& key, const String& val)
{
    this->doPut(const_cast<char*>(key.c_str()), key.size(),
                const_cast<char*>(val.c_str()), val.size());
}

std::string LMDBTransaction::gets(const std::string& key) const
{
    void* valuedata = nullptr;
    size_t valuesize = 0;
    this->doGet(const_cast<char*>(key.data()), key.size(), &valuedata, &valuesize);
    return std::string(reinterpret_cast<const char*>(valuedata), valuesize);
}

void LMDBTransaction::doPut(void* keydata, size_t keysize, void* valuedata, size_t valuesize)
{
    MDB_val key = { keysize, keydata };
    MDB_val val = { valuesize, valuedata };
    lmdb_check(mdb_put(m_txn, m_dbi, &key, &val, 0));
}

void LMDBTransaction::doGet(void* keydata, size_t keysize, void** valuedata, size_t* valuesize) const
{
    MDB_val key{ keysize, keydata };
    MDB_val val{ };

    lmdb_check(mdb_get(m_txn, m_dbi, &key, &val));
    if(valuesize) *valuesize = val.mv_size;
    *valuedata = val.mv_data;
}

} // namespace REDasm
