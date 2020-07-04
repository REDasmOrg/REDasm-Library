#pragma once

#include <exception>

class MDBXException: public std::exception
{
    public:
        MDBXException(int err);
        const char* what() const noexcept override;

    private:
        int m_err{0};
};


inline void mdbx_check(int res) { if(res) throw MDBXException(res); }
