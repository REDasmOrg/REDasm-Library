#include "mdbx.h"
#include "mdbxexception.h"
#include <string>

#define MDBX_MAP_SIZE  4UL * 1024UL * 1024UL * 1024UL // 4GiB

MDBX::~MDBX() { this->close(); }
MDBX::operator bool() const { return m_env; }

void MDBX::open(const std::string& filepath, unsigned int dbflags, unsigned int flags)
{
    if(m_env) return;

    m_dbflags = dbflags;
    mdbx_check(mdbx_env_create(&m_env));
    mdbx_check(mdbx_env_set_geometry(m_env, MDBX_MAP_SIZE, MDBX_MAP_SIZE, MDBX_MAP_SIZE, -1, -1, -1));
    mdbx_check(mdbx_env_open(m_env, filepath.c_str(), flags | MDBX_NOSUBDIR, 0664));
}

MDBXTransactionPtr MDBX::transaction() const { return MDBXTransactionPtr(new MDBXTransaction(m_env, m_dbflags)); }

void MDBX::close()
{
    if(!m_env) return;

    mdbx_dbi_close(m_env, m_dbi);
    mdbx_env_close(m_env);
    m_env = nullptr;
}
