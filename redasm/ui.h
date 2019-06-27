#pragma once

#include <deque>
#include "macros.h"
#include "types/string.h"

namespace REDasm {

namespace UI {
    typedef std::pair<String, bool> CheckListItem;
    typedef std::deque<CheckListItem> CheckList;
} // namespace UI

class LIBREDASM_API AbstractUI
{
    public:
        virtual ~AbstractUI() = default;
        virtual void checkList(const String& title, const String& text, UI::CheckList& items);
        virtual bool askYN(const String& title, const String& text);
};

} // namespace REDasm
