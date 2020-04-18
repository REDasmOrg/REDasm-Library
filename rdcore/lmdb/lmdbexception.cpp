#include "lmdbexception.h"
#include <libs/lmdb/lmdb.h>

LMDBException::LMDBException(int err): m_err(err) { }
const char* LMDBException::what() const noexcept { return mdb_strerror(m_err); }
