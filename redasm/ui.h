#pragma once

#include <string>
#include <deque>
#include "macros.h"

namespace REDasm {

namespace UI {
    typedef std::pair<std::string, bool> CheckListItem;
    typedef std::deque<CheckListItem> CheckList;
} // namespace UI

class LIBREDASM_API AbstractUI
{
    public:
        virtual ~AbstractUI() = default;
        virtual void checkList(const std::string& title, const std::string& text, UI::CheckList& items);
        virtual bool askYN(const std::string& title, const std::string& text);
};

} // namespace REDasm
