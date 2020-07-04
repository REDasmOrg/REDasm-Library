#include "mdbxexception.h"
#include "../libs/mdbx/mdbx.h"

MDBXException::MDBXException(int err): m_err(err) { }
const char* MDBXException::what() const noexcept { return mdbx_strerror(m_err); }
