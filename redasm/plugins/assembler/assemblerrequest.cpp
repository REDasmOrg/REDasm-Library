#include "assemblerrequest.h"
#include <cstring>

namespace REDasm {

AssemblerRequest::AssemblerRequest(): id(nullptr), mode(nullptr) { }
AssemblerRequest::AssemblerRequest(const char *id): id(id), mode(id) { }
bool AssemblerRequest::modeIs(const char *mode) const { return !std::strcmp(this->mode, mode); }
bool AssemblerRequest::idIs(const char *mode) const { return !std::strcmp(this->id, id); }
REDasm::AssemblerRequest::operator const char *() const { return id; }

} // namespace REDasm
