#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <string>

namespace REDasm {
namespace Demangler {

bool isMSVC(const std::string &s, std::string *result = NULL);
bool isMangled(const std::string& s);
std::string demangled(const std::string& s, bool simplified = true);

} // namespace Demangler
} // namespace REDasm

#endif // DEMANGLER_H
