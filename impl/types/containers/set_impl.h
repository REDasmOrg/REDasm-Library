#pragma once

#include <redasm/types/containers/set.h>
#include "templates/sortedset_template.h"
#include "templates/set_template.h"

namespace REDasm {

class SetImpl: public SetTemplate<Variant>
{
    public:
        SetImpl();
};

class SortedSetImpl: public SortedSetTemplate<Variant>
{
    public:
        SortedSetImpl();
};

} // namespace REDasm
