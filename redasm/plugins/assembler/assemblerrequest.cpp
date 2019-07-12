#include "assemblerrequest.h"
#include <cstring>

namespace REDasm {

AssemblerRequest::AssemblerRequest() { }
AssemblerRequest::AssemblerRequest(const char *id, const char *mode): id(id) { this->mode = mode ? mode : id; }
bool AssemblerRequest::modeIs(const String &mode) const { return this->mode == mode; }
bool AssemblerRequest::idIs(const String& id) const { return this->id == id; }
REDasm::AssemblerRequest::operator String() const { return id; }

} // namespace REDasm
