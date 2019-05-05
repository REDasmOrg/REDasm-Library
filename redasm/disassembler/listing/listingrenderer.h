#pragma once

#include "../../plugins/assembler/printer.h"
#include "listingdocument.h"

#define REDASM_WORD_REGEX    "([\\w\\$_\\.]+)"

namespace REDasm {

struct RendererFormat
{
    s64 start, end; // [start, end]
    std::string fgstyle, bgstyle;

    inline bool empty() const { return start > end; }
    inline s64 length() const { return (end - start + 1); }
    inline bool contains(s64 pos) const { return (pos >= start) && (pos <= end); }
    inline bool equals(s64 start, s64 end) const { return (start == this->start) && (end == this->end); }
};

struct RendererLine
{
    RendererLine(bool ignoreflags = false): userdata(nullptr), documentindex(0), index(0), highlighted(false), ignoreflags(ignoreflags) { }

    void* userdata;
    size_t documentindex, index;
    bool highlighted, ignoreflags;
    std::list<RendererFormat> formats;
    std::string text;

    std::string formatText(const RendererFormat& rf) const { return text.substr(rf.start, rf.length()); }
    size_t length() const { return text.length(); }

    std::list<RendererFormat>::iterator unformat(s64 start, s64 end) {
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

    RendererLine& format(s64 start, s64 end, const std::string& fgstyle = std::string(), const std::string& bgstyle = std::string()) {
        if(text.empty() || (start >= static_cast<s64>(text.size())))
            return *this;

        end = std::min(end, static_cast<s64>(text.size() - 1));

        auto it = this->unformat(start, end);
        formats.insert(it, { start, end, fgstyle, bgstyle });
        return *this;
    }

    RendererLine& push(const std::string& text, const std::string& fgstyle = std::string(), const std::string& bgstyle = std::string()) {
        s64 start = static_cast<s64>(this->text.size());
        formats.push_back({ start, start + static_cast<s64>(text.length()) - 1, fgstyle, bgstyle});
        this->text += text;
        return *this;
    }
};

class ListingRenderer
{
    public:
        typedef std::pair<s64, s64> Range;
        enum: u32 { Normal = 0, HideSegmentName = 1, HideAddress = 2,
                    HideSegmentAndAddress = HideSegmentName | HideAddress };

    public:
        ListingRenderer(DisassemblerAPI* disassembler);
        virtual ~ListingRenderer() = default;
        virtual void render(size_t start, size_t count, void* userdata = nullptr);
        DisassemblerAPI* disassembler() const;
        const ListingDocument& document() const;
        const REDasm::Symbol* symbolUnderCursor();
        ListingDocument& document();
        void setFlags(u32 flags);
        std::string wordFromPosition(const ListingCursor::Position& pos, ListingRenderer::Range *wordpos = nullptr);
        std::string getCurrentWord();
        u64 getLastColumn(u64 line);
        std::string getLine(u64 line);
        std::string getSelectedText();

    protected:
        virtual void renderLine(const RendererLine& rl) = 0;
        bool hasFlag(u32 flag) const;
        bool getRendererLine(size_t line, RendererLine& rl);
        void renderSegment(const document_s_lock& lock, const ListingItem *item, RendererLine& rl);
        void renderFunction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderInstruction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderSymbol(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMeta(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderType(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddress(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMnemonic(const InstructionPtr& instruction, RendererLine &rl);
        void renderOperands(const InstructionPtr& instruction, RendererLine &rl);
        void renderComments(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddressIndent(const document_s_lock &lock, const ListingItem *item, RendererLine& rl);
        void renderIndent(RendererLine &rl, int n = 1);

    private:
        bool renderSymbolPointer(const document_s_lock &lock, const Symbol *symbol, RendererLine& rl) const;
        bool getRendererLine(const document_s_lock& lock, size_t line, RendererLine& rl);
        void highlightSelection(RendererLine& rl);
        void blinkCursor(RendererLine& rl);
        void highlightWord(RendererLine& rl, const std::string word);
        static std::string escapeString(const std::string& s);

    protected:
        ListingDocument& m_document;
        DisassemblerAPI* m_disassembler;
        ListingCursor* m_cursor;

    private:
        u32 m_flags;
        PrinterPtr m_printer;
};

} // namespace REDasm
