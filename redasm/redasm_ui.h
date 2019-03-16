#ifndef REDASM_UI_H
#define REDASM_UI_H

#include <string>

namespace REDasm {

class AbstractUI
{
    public:
        virtual ~AbstractUI() = default;
        virtual bool askYN(const std::string& title, const std::string& text);
};

} // namespace REDasm

#endif // REDASM_UI_H
