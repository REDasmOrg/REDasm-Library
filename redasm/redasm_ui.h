#ifndef REDASM_UI_H
#define REDASM_UI_H

#include <string>
#include <deque>

namespace REDasm {

namespace UI {
    typedef std::pair<std::string, bool> CheckListItem;
    typedef std::deque<CheckListItem> CheckList;
} // namespace UI

class AbstractUI
{
    public:
        virtual ~AbstractUI() = default;
        virtual void checkList(const std::string& title, const std::string& text, UI::CheckList& items);
        virtual bool askYN(const std::string& title, const std::string& text);
};

} // namespace REDasm

#endif // REDASM_UI_H
