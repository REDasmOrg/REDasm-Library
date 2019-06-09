#pragma once

#include <stack>
#include "../../support/event.h"
#include "../../support/serializer.h"
#include "../../pimpl.h"

namespace REDasm {

class ListingCursorImpl;

class LIBREDASM_API ListingCursor
{
    PIMPL_DECLARE_P(ListingCursor)
    PIMPL_DECLARE_PRIVATE(ListingCursor)

    public:
        typedef std::pair<size_t, size_t> Position;       // [Line, Column]

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
        size_t currentLine() const;
        size_t currentColumn() const;
        size_t selectionLine() const;
        size_t selectionColumn() const;
        bool isLineSelected(size_t line) const;
        bool hasSelection() const;
        bool canGoBack() const;
        bool canGoForward() const;
        void set(size_t line, size_t column = 0);
        void moveTo(size_t line, size_t column = 0);
        void select(size_t line, size_t column = 0);
        void goBack();
        void goForward();

    friend class Serializer<ListingCursor>;
};

template<> struct Serializer<ListingCursor> {
    static void write(std::fstream& fs, const ListingCursor* c) {
        Serializer<size_t>::write(fs, c->currentLine());
        Serializer<size_t>::write(fs, c->currentColumn());
    }

    static void read(std::fstream& fs, ListingCursor* c) {
        size_t line = 0, column = 0;
        Serializer<size_t>::read(fs, line);
        Serializer<size_t>::read(fs, column);
        c->set(line, column);
    }
};

} // namespace REDasm
