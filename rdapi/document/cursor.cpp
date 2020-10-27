#include "cursor.h"
#include <rdcore/renderer/cursor.h>

RDCursor* RDCursor_Create(RDDocument* d) { return CPTR(RDCursor, new Cursor(nullptr)); }
void RDCursor_GoBack(RDCursor* c) { CPTR(Cursor, c)->goBack(); }
void RDCursor_GoForward(RDCursor* c) { CPTR(Cursor, c)->goForward(); }
void RDCursor_Enable(RDCursor* c) { CPTR(Cursor, c)->enable(); }
void RDCursor_Disable(RDCursor* c) { CPTR(Cursor, c)->disable(); }
void RDCursor_Toggle(RDCursor* c) { CPTR(Cursor, c)->toggle(); }
void RDCursor_ClearSelection(RDCursor* c) { CPTR(Cursor, c)->clearSelection(); }
void RDCursor_MoveTo(RDCursor* c, size_t line, size_t column) { CPTR(Cursor, c)->moveTo(line, column); }
void RDCursor_Select(RDCursor* c, size_t line, size_t column) { CPTR(Cursor, c)->select(line, column); }
bool RDCursor_IsLineSelected(const RDCursor* c, size_t line) { return CPTR(const Cursor, c)->isRowSelected(line); }
bool RDCursor_HasSelection(const RDCursor* c) { return CPTR(const Cursor, c)->hasSelection(); }
bool RDCursor_CanGoBack(const RDCursor* c) { return CPTR(const Cursor, c)->canGoBack(); }
bool RDCursor_CanGoForward(const RDCursor* c) { return CPTR(const Cursor, c)->canGoForward(); }
size_t RDCursor_CurrentLine(const RDCursor* c) { return CPTR(const Cursor, c)->currentRow(); }
size_t RDCursor_CurrentColumn(const RDCursor* c) { return CPTR(const Cursor, c)->currentColumn(); }
size_t RDCursor_SelectionLine(const RDCursor* c) { return CPTR(const Cursor, c)->selectionLine();  }
size_t RDCursor_SelectionColumn(const RDCursor* c) { return CPTR(const Cursor, c)->selectionColumn(); }
const RDCursorPos* RDCursor_GetPosition(const RDCursor* c) { return CPTR(const Cursor, c)->position(); }
const RDCursorPos* RDCursor_GetSelection(const RDCursor* c) { return CPTR(const Cursor, c)->selection(); }
const RDCursorPos* RDCursor_GetStartSelection(const RDCursor* c) { return CPTR(const Cursor, c)->startSelection(); }
const RDCursorPos* RDCursor_GetEndSelection(const RDCursor* c) { return CPTR(const Cursor, c)->endSelection(); }
