#pragma once

#define SEPARATOR_LENGTH 50

#include <redasm/disassembler/listing/listingrenderer.h>
#include <redasm/pimpl.h>
#include "../../types/containers/templates/list_template.h"

namespace REDasm {

class RendererFormatListImpl: public ListTemplate<RendererFormat> { };

class ListingRendererImpl
{
    PIMPL_DECLARE_Q(ListingRenderer)
    PIMPL_DECLARE_PUBLIC(ListingRenderer)

    public:
        ListingRendererImpl(ListingRenderer* q);
        bool renderSymbolPointer(const document_s_lock_new &lock, const Symbol *symbol, RendererLine& rl) const;
        void renderSymbolPrologue(const document_s_lock_new &lock, const ListingItem& item, const Symbol *symbol, RendererLine& rl) const;
        void renderBlockBytes(const BlockItem* bi, RendererLine& rl) const;
        bool getRendererLine(const document_s_lock_new& lock, size_t line, RendererLine& rl) const;
        void highlightSelection(RendererLine& rl);
        void blinkCursor(RendererLine& rl);
        void highlightWord(RendererLine& rl, const String word);
        bool hasFlag(ListingRendererFlags flag) const;
        void setFlags(ListingRendererFlags flags);
        static String escapeString(const String& s);

    private:
        void renderPrologue(const document_s_lock_new &lock, const ListingItem& item, RendererLine& rl) const;

    private:
        object_ptr<Printer> m_printer;
        ListingRendererFlags m_flags{ListingRendererFlags::Normal};
};

} // namespace REDasm
