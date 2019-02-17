#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <string>
#include <typeinfo>

namespace REDasm {
namespace Demangler {

bool isMSVC(const std::string &s, std::string *result = nullptr);
bool isMangled(const std::string& s);
std::string demangled(const std::string& s, bool simplified = true);
template<typename T> std::string typeName() { return demangled(typeid(T).name()); }

} // namespace Demangler
} // namespace REDasm

#endif // DEMANGLER_H
