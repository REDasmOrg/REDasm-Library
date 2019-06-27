#pragma once

#include "../../plugins/assembler/printer/printer.h"
#include "listingdocument.h"

#define REDASM_WORD_REGEX    "([\\w\\$_\\.]+)"

namespace REDasm {

struct RendererFormat
{
    size_t start, end; // [start, end]
    String fgstyle, bgstyle;

    inline size_t length() const {
        if((start == REDasm::npos) || (end == REDasm::npos))
            return 0;

        return start <= end ? (end - start + 1) : 0;
    }

    inline bool empty() const { return this->length() == 0; }
    inline bool contains(size_t pos) const { return (pos >= start) && (pos <= end); }
    inline bool equals(size_t start, size_t end) const { return (start == this->start) && (end == this->end); }
};

struct RendererLine
{
    RendererLine(bool ignoreflags = false): userdata(nullptr), documentindex(0), index(0), highlighted(false), ignoreflags(ignoreflags) { }

    void* userdata;
    size_t documentindex, index;
    bool highlighted, ignoreflags;
    std::list<RendererFormat> formats;
    String text;

    String formatText(const RendererFormat& rf) const { return text.substring(rf.start, rf.length()); }
    size_t length() const { return text.size(); }

    std::list<RendererFormat>::iterator unformat(size_t start, size_t end) {
        auto begit = std::find_if(formats.begin(), formats.end(), [=](const RendererFormat& rf) -> bool { return rf.contains(start); });
        auto endit = std::find_if(formats.begin(), formats.end(), [=](const RendererFormat& rf) -> bool { return rf.contains(end); });

        RendererFormat begrf = *begit, endrf = *endit;
        auto it = formats.erase(begit, ++endit);

        begrf.end = start - 1; // Shrink first part
        endrf.start = end + 1; // Shrink last part

        if(!begrf.empty())
        {
            it = formats.insert(it, begrf);
            it++;
        }

        if(!endrf.empty())
            it = formats.insert(it, endrf);

        return it;
    }

    RendererLine& format(size_t start, size_t end, const String& fgstyle = String(), const String& bgstyle = String()) {
        if(text.empty() || (start >= text.size()))
            return *this;

        end = std::min(end, text.size() - 1);

        auto it = this->unformat(start, end);
        formats.insert(it, { start, end, fgstyle, bgstyle });
        return *this;
    }

    RendererLine& push(const String& text, const String& fgstyle = String(), const String& bgstyle = String()) {
        size_t start = this->text.size();
        formats.push_back({ start, start + text.size() - 1, fgstyle, bgstyle});
        this->text += text;
        return *this;
    }
};

enum class ListingRendererFlags
{
    Normal = 0,
    HideSegmentName,
    HideAddress,
    HideSegmentAndAddress = HideSegmentName | HideAddress
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
        ListingRenderer(Disassembler* disassembler);
        virtual ~ListingRenderer() = default;
        virtual void render(size_t start, size_t count, void* userdata = nullptr);
        Disassembler* disassembler() const;
        const ListingDocument& document() const;
        const REDasm::Symbol* symbolUnderCursor();
        ListingDocument& document();
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
        void renderSegment(const document_s_lock& lock, const ListingItem *item, RendererLine& rl);
        void renderFunction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderInstruction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderSymbol(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMeta(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderType(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddress(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMnemonic(const CachedInstruction& instruction, RendererLine &rl);
        void renderOperands(const CachedInstruction& instruction, RendererLine &rl);
        void renderComments(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddressIndent(const document_s_lock &lock, const ListingItem *item, RendererLine& rl);
        void renderIndent(RendererLine &rl, int n = 1);
        Printer* printer() const;
        ListingCursor* cursor() const;
};

} // namespace REDasm
