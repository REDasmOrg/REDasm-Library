#pragma once

#include <redasm/disassembler/listing/listingrenderer.h>
#include <redasm/pimpl.h>

namespace REDasm {

class ListingRendererImpl
{
    PIMPL_DECLARE_PUBLIC(ListingRenderer)

    public:
        ListingRendererImpl(ListingRenderer* q, Disassembler* disassembler);
        bool renderSymbolPointer(const document_s_lock &lock, const Symbol *symbol, RendererLine& rl) const;
        bool getRendererLine(const document_s_lock& lock, size_t line, RendererLine& rl);
        void highlightSelection(RendererLine& rl);
        void blinkCursor(RendererLine& rl);
        void highlightWord(RendererLine& rl, const std::string word);
        bool hasFlag(ListingRendererFlags flag) const;
        void setFlags(ListingRendererFlags flags);
        static std::string escapeString(const std::string& s);

    private:
        object_ptr<Printer> m_printer;
        ListingDocument& m_document;
        Disassembler* m_disassembler;
        ListingCursor* m_cursor;
        ListingRendererFlags m_flags;
};

} // namespace REDasm
