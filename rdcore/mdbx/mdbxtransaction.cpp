#include "mdbxtransaction.h"
#include "mdbxexception.h"
#include <string>

MDBXTransaction::MDBXTransaction(MDBX_env* env, unsigned int dbflags): m_env(env)
{
    mdbx_check(mdbx_txn_begin(env, nullptr, 0, &m_txn));
    mdbx_check(mdbx_dbi_open(m_txn, nullptr, dbflags, &m_dbi));
}

MDBXTransaction::~MDBXTransaction()
{
    if(!m_txn) return;
    mdbx_dbi_close(m_env, m_dbi);
}

void MDBXTransaction::commit() { mdbx_check(mdbx_txn_commit(m_txn)); }
void MDBXTransaction::abort() { mdbx_txn_abort(m_txn); }

void MDBXTransaction::puts(const std::string& key, const std::string& val)
{
    this->doPut(const_cast<char*>(key.c_str()), key.size(),
                const_cast<char*>(val.c_str()), val.size());
}

std::string MDBXTransaction::gets(const std::string& key) const
{
    void* valuedata = nullptr;
    size_t valuesize = 0;
    this->doGet(const_cast<char*>(key.data()), key.size(), &valuedata, &valuesize);
    return std::string(reinterpret_cast<const char*>(valuedata), valuesize);
}

void MDBXTransaction::doPut(void* keydata, size_t keysize, void* valuedata, size_t valuesize)
{
    MDBX_val key = { keydata, keysize };
    MDBX_val val = { valuedata, valuesize };
    mdbx_check(mdbx_put(m_txn, m_dbi, &key, &val, 0));
}

void MDBXTransaction::doGet(void* keydata, size_t keysize, void** valuedata, size_t* valuesize) const
{
    MDBX_val key{ keydata, keysize };
    MDBX_val val{ };

    mdbx_check(mdbx_get(m_txn, m_dbi, &key, &val));
    if(valuesize) *valuesize = val.iov_len;
    *valuedata = val.iov_base;
}
