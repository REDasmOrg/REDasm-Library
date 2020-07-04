#pragma once

#include "../libs/mdbx/mdbx.h"
#include "mdbxtransaction.h"

class MDBX
{
    public:
        MDBX() = default;
        ~MDBX();

    public:
        operator bool() const;
        void open(const std::string& filepath, unsigned int dbflags = 0, unsigned int flags = 0u);
        MDBXTransactionPtr transaction() const;
        void close();

    private:
        unsigned int m_dbflags{0};
        MDBX_env* m_env{nullptr};
        MDBX_dbi m_dbi;
};

