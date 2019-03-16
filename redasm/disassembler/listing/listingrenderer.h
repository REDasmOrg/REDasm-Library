#ifndef LISTINGRENDERER_H
#define LISTINGRENDERER_H

#include "../../plugins/assembler/printer.h"
#include "listingdocument.h"

#define REDASM_WORD_REGEX    "([\\w\\$_\\.]+)"

namespace REDasm {

struct RendererFormat
{
    RendererFormat(s64 start, s64 length, const std::string& style): start(start), length(length), style(style) { }
    s64 start, length;
    std::string style;

    bool contains(s64 pos) const { return (pos >= start) && (pos < (start + length)); }
};

struct RendererLine
{
    RendererLine(): userdata(nullptr), documentindex(0), index(0), highlighted(false) { }

    void* userdata;
    u64 documentindex, index;
    bool highlighted;
    std::list<RendererFormat> formats;
    std::string text;

    std::string formatText(const RendererFormat& rf) const { return text.substr(rf.start, rf.start + rf.length - 1); }
    size_t length() const { return text.length(); }

    RendererLine& push(const std::string& text, const std::string& style = std::string()) {
        formats.emplace_back(this->text.size(), text.length(), style);
        this->text += text;
        return *this;
    }
};

class ListingRenderer
{
    protected:
        enum: u32 { Normal = 0, HideSegmentName = 1, HideAddress = 2,
                    HideSegmentAndAddress = HideSegmentName | HideAddress
                  };

    public:
        ListingRenderer(DisassemblerAPI* disassembler);
        virtual void render(u64 start, u64 count, void* userdata = nullptr);
        std::string wordFromPosition(const ListingCursor::Position& pos);
        u64 getLastColumn(u64 line);
        std::string getLine(u64 line);
        std::string getSelectedText();

    protected:
        virtual void renderLine(const RendererLine& rl) = 0;
        void setFlags(u32 flags);
        bool getRendererLine(u64 line, RendererLine& rl);
        void renderSegment(const document_s_lock& lock, const ListingItem *item, RendererLine& rl);
        void renderFunction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderInstruction(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderSymbol(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMeta(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderType(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddress(const document_s_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMnemonic(const InstructionPtr& instruction, RendererLine &rl);
        void renderOperands(const InstructionPtr& instruction, RendererLine &rl);
        void renderComments(const document_s_lock &lock, const InstructionPtr& instruction, RendererLine &rl);
        void renderAddressIndent(const document_s_lock &lock, const ListingItem *item, RendererLine& rl);
        void renderIndent(RendererLine &rl, int n = 1);

    private:
        bool renderSymbolPointer(const document_s_lock &lock, const Symbol *symbol, RendererLine& rl) const;
        bool getRendererLine(const document_s_lock& lock, u64 line, RendererLine& rl);
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

#endif // LISTINGRENDERER_H
