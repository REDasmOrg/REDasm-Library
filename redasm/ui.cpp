#include "ui.h"

namespace REDasm {

void AbstractUI::checkList(const String &title, const String &text, UI::CheckList &items) { }
bool AbstractUI::askYN(const String &title, const String &text) { return false; }

} // namespace REDasm
