#pragma once

#include <redasm/types/variant.h>
#include <redasm/types/containers/set.h>
#include <unordered_map>
#include <map>
#include "templates/dictionary_template.h"

namespace REDasm {

class DictionaryImpl: public DictionaryTemplate< std::unordered_map<Variant, Variant>, Set > { };
class SortedDictionaryImpl: public DictionaryTemplate< std::map<Variant, Variant>, SortedSet > { };

} // namespace REDasm
