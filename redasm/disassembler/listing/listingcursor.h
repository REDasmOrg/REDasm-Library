#pragma once

#include "../../types/object.h"
#include "../../macros.h"
#include "../../pimpl.h"

namespace REDasm {

class ListingCursorImpl;

class LIBREDASM_API ListingCursor: public Object
{
    REDASM_OBJECT(ListingCursor)
    PIMPL_DECLARE_P(ListingCursor)
    PIMPL_DECLARE_PRIVATE(ListingCursor)

    public:
        struct Position {
           size_t line, column;
           bool operator ==(const Position& rhs) const;
           bool operator !=(const Position& rhs) const;
        };

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

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
};

} // namespace REDasm
