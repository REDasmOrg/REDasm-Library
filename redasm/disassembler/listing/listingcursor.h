#pragma once

#include <stack>
#include "../../redasm.h"
#include "../../support/event.h"

namespace REDasm {

class ListingCursor
{
    public:
        typedef std::pair<u64, u64> Position;       // [Line, Column]
        typedef std::stack<Position> PositionStack;

    public:
        SimpleEvent positionChanged;
        SimpleEvent backChanged;
        SimpleEvent forwardChanged;

    public:
        ListingCursor();
        bool active() const;
        void toggle();
        void enable();
        void disable();
        void clearSelection();
        const ListingCursor::Position& currentPosition() const;
        const ListingCursor::Position& currentSelection() const;
        const ListingCursor::Position& startSelection() const;
        const ListingCursor::Position& endSelection() const;
        u64 currentLine() const;
        u64 currentColumn() const;
        u64 selectionLine() const;
        u64 selectionColumn() const;
        bool isLineSelected(u64 line) const;
        bool hasSelection() const;
        bool canGoBack() const;
        bool canGoForward() const;
        void set(u64 line, u64 column = 0);
        void moveTo(u64 line, u64 column = 0);
        void select(u64 line, u64 column = 0);
        void goBack();
        void goForward();

    private:
        void moveTo(u64 line, u64 column, bool save);

    private:
        Position m_position, m_selection;
        PositionStack m_backstack, m_forwardstack;
        bool m_active;
};

} // namespace REDasm
