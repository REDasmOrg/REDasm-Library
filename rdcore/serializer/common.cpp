#include "common.h"

const std::string& SerializerCommon::lastError() const { return m_lasterror; }
bool SerializerCommon::setLastError(const std::string& s) { m_lasterror = s; return false; }
void SerializerCommon::clearLastError() { m_lasterror.clear(); }
