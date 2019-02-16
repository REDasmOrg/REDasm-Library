#ifndef LISTINGRENDERER_H
#define LISTINGRENDERER_H

#include "../../plugins/assembler/printer.h"
#include "listingdocument.h"

namespace REDasm {

using document_lock = s_locked_safe_ptr<ListingDocument>;

struct RendererFormat
{
    RendererFormat(s64 start, s64 length, const std::string& style): start(start), length(length), style(style) { }
    s64 start, length;
    std::string style;
};

struct RendererLine
{
    RendererLine(): userdata(nullptr), documentindex(0), index(0), highlighted(false) { }

    void* userdata;
    u64 documentindex, index;
    bool highlighted;
    std::list<RendererFormat> formats;
    std::string text;

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
        u64 getLastColumn(u64 line);
        std::string getLine(u64 line);
        std::string getSelectedText();

    protected:
        virtual void renderLine(const RendererLine& rl) = 0;
        void setFlags(u32 flags);
        bool getRendererLine(size_t line, RendererLine& rl);
        void renderSegment(const document_lock& lock, const ListingItem *item, RendererLine& rl);
        void renderFunction(const document_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderInstruction(const document_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderSymbol(const document_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderInfo(const document_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderAddress(const document_lock &lock, const ListingItem *item, RendererLine &rl);
        void renderMnemonic(const InstructionPtr& instruction, RendererLine &rl);
        void renderOperands(const InstructionPtr& instruction, RendererLine &rl);
        void renderComments(const document_lock &lock, const InstructionPtr& instruction, RendererLine &rl);
        void renderAddressIndent(const document_lock &lock, const ListingItem *item, RendererLine& rl);
        void renderIndent(RendererLine &rl, int n = 1);

    private:
        void renderTable(const document_lock &lock, const SymbolPtr &symbol, RendererLine &rl) const;
        bool renderSymbolPointer(const document_lock &lock, const SymbolPtr& symbol, RendererLine& rl) const;
        bool getRendererLine(const document_lock& lock, size_t line, RendererLine& rl);
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
