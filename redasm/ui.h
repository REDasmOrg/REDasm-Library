#pragma once

#include <deque>
#include "macros.h"
#include "types/string.h"
#include "types/containers/list.h"

namespace REDasm {

namespace UI {
    typedef std::pair<String, bool> CheckListItem;
    typedef std::deque<CheckListItem> CheckList;
} // namespace UI

class LIBREDASM_API AbstractUI
{
    public:
        virtual ~AbstractUI() = default;
        virtual int select(const REDasm::String &title, const REDasm::String& text, const REDasm::List &items);
        virtual void checkList(const String& title, const String& text, UI::CheckList& items);
        virtual bool askYN(const String& title, const String& text);
};

} // namespace REDasm
