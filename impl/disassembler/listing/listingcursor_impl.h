#pragma once

#include <redasm/disassembler/listing/listingcursor.h>
#include <redasm/pimpl.h>
#include <stack>

namespace REDasm {

class ListingCursorImpl
{
    PIMPL_DECLARE_Q(ListingCursor)
    PIMPL_DECLARE_PUBLIC(ListingCursor)

    public:
        typedef std::stack<ListingCursor::Position> PositionStack;

    public:
        ListingCursorImpl(ListingCursor* q);
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        void moveTo(size_t line, size_t column, bool save);

    private:
        ListingCursor::Position m_position, m_selection;
        PositionStack m_backstack, m_forwardstack;
        bool m_active;
};

} // namespace REDasm
