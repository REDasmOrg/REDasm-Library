#ifndef SUPPORT_RELOCATION_H
#define SUPPORT_RELOCATION_H

#include <deque>
#include "../types/base_types.h"

namespace REDasm {

struct RelocationItem {
    offset_t offset;
    u64 size;
};

typedef std::deque<RelocationItem> RelocationList;

} // namespace REDasm

#endif // SUPPORT_RELOCATION_H
