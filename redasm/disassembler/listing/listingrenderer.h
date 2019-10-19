#pragma once

#include "../../plugins/assembler/printer/printer.h"
#include "listingdocumentnew.h"
#include "listingdocument.h"

#define REDASM_WORD_REGEX    "([\\w\\$_\\.]+)"

namespace REDasm {

struct RendererFormat
{
    size_t start, end; // [start, end]
    String fgstyle, bgstyle;

    size_t length() const;
    bool empty() const;
    bool contains(size_t pos) const;
    bool equals(size_t start, size_t end) const;
};

class RendererFormatListImpl;

class RendererFormatList
{
    PIMPL_DECLARE_P(RendererFormatList)
    PIMPL_DECLARE_PRIVATE(RendererFormatList)

    public:
        RendererFormatList();
        const RendererFormat& at(size_t pos) const;
        RendererFormat& at(size_t pos);
        size_t indexFromPos(size_t pos) const;
        size_t size() const;
        void append(const RendererFormat& rf);
        size_t insert(size_t idx, const RendererFormat& rf);
        size_t erase(size_t start, size_t end);
};

struct RendererLine
{
    void* userdata;
    size_t documentindex, index;
    bool highlighted, ignoreflags;
    RendererFormatList formats;
    String text;

    RendererLine(bool ignoreflags = false);
    String formatText(const RendererFormat& rf) const;
    size_t length() const;
    size_t unformat(size_t start, size_t end);
    RendererLine& format(size_t start, size_t end, const String& fgstyle = String(), const String& bgstyle = String());
    RendererLine& push(const String& text, const String& fgstyle = String(), const String& bgstyle = String());
};

enum class ListingRendererFlags
{
    Normal          = 0,
    HideSegment     = (1 << 0),
    HideAddress     = (1 << 1),
    HideSeparators  = (1 << 2),

    HideSegmentAndAddress = HideSegment | HideAddress
};

ENUM_FLAGS_OPERATORS(ListingRendererFlags)

class ListingRendererImpl;

class ListingRenderer
{
    PIMPL_DECLARE_P(ListingRenderer)
    PIMPL_DECLARE_PRIVATE(ListingRenderer)

    public:
        typedef std::pair<size_t, size_t> Range;

    public:
        ListingRenderer();
        virtual ~ListingRenderer() = default;
        virtual void render(size_t start, size_t count, void* userdata = nullptr);
        const REDasm::Symbol* symbolUnderCursor();
        void setFlags(ListingRendererFlags flags);
        String wordFromPosition(const ListingCursor::Position& pos, ListingRenderer::Range *wordpos = nullptr);
        String getCurrentWord();
        size_t getLastColumn(size_t line);
        String getLine(size_t line);
        String getSelectedText();

    protected:
        virtual void renderLine(const RendererLine& rl) = 0;
        bool hasFlag(ListingRendererFlags flag) const;
        bool getRendererLine(size_t line, RendererLine& rl);
        void renderSegment(const document_s_lock_new& lock, const ListingItem& item, RendererLine& rl);
        void renderFunction(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderInstruction(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderSymbol(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl);
        void renderMeta(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderType(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl);
        void renderSeparator(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderUnexplored(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderAddress(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl);
        void renderMnemonic(const CachedInstruction& instruction, RendererLine &rl);
        void renderOperands(const CachedInstruction& instruction, RendererLine &rl);
        void renderComments(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl);
        void renderAddressIndent(const document_s_lock_new &lock, const ListingItem& item, RendererLine& rl);
        void renderIndent(RendererLine &rl, int n = 1);
        Printer* printer() const;
};

} // namespace REDasm
