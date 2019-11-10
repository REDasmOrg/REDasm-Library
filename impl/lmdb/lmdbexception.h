#pragma once

#include <exception>

namespace REDasm {

class LMDBException: public std::exception
{
    public:
        LMDBException(int err);
        const char* what() const noexcept override;

    private:
        int m_err{0};
};


inline void lmdb_check(int res) { if(res) throw LMDBException(res); }

} // namespace REDasm

