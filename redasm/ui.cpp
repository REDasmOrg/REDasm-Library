#include "ui.h"

namespace REDasm {

void AbstractUI::checkList(const std::string &title, const std::string &text, UI::CheckList &items) { }
bool AbstractUI::askYN(const std::string &title, const std::string &text) { return false; }

} // namespace REDasm
