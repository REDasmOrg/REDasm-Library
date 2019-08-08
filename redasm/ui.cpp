#include "ui.h"

namespace REDasm {

int AbstractUI::select(const String &title, const String &text, const List &items) { return -1; }
void AbstractUI::checkList(const String &title, const String &text, UI::CheckList &items) { }
bool AbstractUI::askYN(const String &title, const String &text) { return false; }

} // namespace REDasm
