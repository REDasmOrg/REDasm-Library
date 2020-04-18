#pragma once

#include "../object.h"
#include "document_fwd.h"
#include <rdapi/document/cursor.h>
#include <stack>

class Cursor: public Object
{
    public:
        typedef std::stack<RDCursorPos> PositionStack;

    public:
        Cursor(SafeDocument& d);
        void toggle();
        void enable();
        void disable();
        void goBack();
        void goForward();
        void clearSelection();
        void set(size_t line, size_t column);
        void moveTo(size_t line, size_t column);
        void select(size_t line, size_t column);

    public:
        const RDCursorPos* position() const;
        const RDCursorPos* selection() const;
        const RDCursorPos* startSelection() const;
        const RDCursorPos* endSelection() const;
        size_t currentLine() const;
        size_t currentColumn() const;
        size_t selectionLine() const;
        size_t selectionColumn() const;
        bool isLineSelected(size_t line) const;
        bool hasSelection() const;
        bool canGoBack() const;
        bool canGoForward() const;
        bool active() const;

    private:
        void moveTo(size_t line, size_t column, bool save);
        static bool equalPos(const RDCursorPos* pos1, const RDCursorPos* pos2);

    private:
        bool m_active{false};
        RDCursorPos m_position{0, 0}, m_selection{0, 0};
        PositionStack m_backstack, m_forwardstack;
        SafeDocument& m_document;
};

