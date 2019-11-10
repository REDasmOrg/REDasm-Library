#pragma once

#include "../libs/lmdb/lmdb.h"
#include "lmdbtransaction.h"
#include <redasm/types/string.h>

namespace REDasm {

class LMDB
{
    public:
        LMDB() = default;
        ~LMDB();

    public:
        operator bool() const;
        void open(const String& filepath, unsigned int dbflags = 0, unsigned int flags = 0u);
        LMDBTransactionPtr transaction() const;
        void close();

    private:
        unsigned int m_dbflags{0};
        MDB_env* m_env{nullptr};
        MDB_dbi m_dbi;
};

} // namespace REDasm

