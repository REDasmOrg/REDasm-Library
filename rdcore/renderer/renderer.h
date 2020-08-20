#pragma once

#include "../disassembler.h"
#include <rdapi/document/document.h>
#include <rdapi/renderer.h>
#include "rendereritem.h"
#include "../object.h"

struct RDCursorRange;
class Disassembler;
class Cursor;

class Renderer: public Object
{
    private:
        typedef bool (*Renderer_Callback)(RDRenderItemParams* rip);

    public:
        Renderer(Disassembler* disassembler, const Cursor* cursor, rd_flag flags);
        void render(size_t index, size_t count, Callback_Render cbrender, void* userdata) const;
        bool renderItem(size_t index, RendererItem* ritem) const;
        bool selectedSymbol(RDSymbol* symbol) const;
        const std::string& getWordFromPosition(const RDCursorPos* pos, RDCursorRange* range) const;
        const std::string& getInstructionText(rd_address address) const;
        const std::string& getAssemblerInstruction(rd_address address) const;
        const std::string& getRDILInstruction(rd_address address) const;
        const std::string& getSelectedText() const;
        const std::string& getCurrentWord() const;
        size_t getLastColumn(size_t index) const;
        rd_flag flags() const;

    public:
        void renderHexDump(RendererItem* ritem, const RDBufferView* view, size_t size) const;
        void renderAddress(RendererItem* ritem, rd_address address) const;
        void renderMnemonic(RendererItem* ritem, const std::string& s, rd_type theme) const;
        void renderConstant(RendererItem* ritem, const std::string& s) const;
        void renderRegister(RendererItem* ritem, const std::string& s) const;
        void renderText(RendererItem* ritem, const std::string& s, rd_type theme = Theme_Default) const;
        void renderIndent(RendererItem* ritem, size_t n, bool ignoreflags = false) const;
        void renderAssemblerInstruction(rd_address address, RendererItem* ritem) const;
        void renderRDILInstruction(rd_address address, RendererItem* ritem) const;

    private:
        void renderSegment(rd_address address, RendererItem* ritem) const;
        void renderFunction(rd_address address, RendererItem* ritem) const;
        void renderInstruction(rd_address address, RendererItem* ritem) const;
        void renderSymbol(rd_address address, RendererItem* ritem) const;

    private:
        void compileParams(rd_address address, RendererItem* ritem, RDRenderItemParams* rip) const;
        void highlightSelection(RendererItem* ritem) const;
        void highlightCursor(RendererItem* ritem) const;
        void highlightWords(RendererItem* ritem) const;
        void renderSeparator(rd_address address, RendererItem* ritem) const;
        void renderUnexplored(rd_address address, RendererItem* ritem) const;
        void renderAddressIndent(rd_address address, RendererItem* ritem) const;
        void renderPrologue(rd_address address, RendererItem* ritem) const;
        void renderComments(rd_address address, RendererItem* ritem) const;
        void renderBlock(rd_address address, RendererItem* ritem) const;
        void renderSymbolValue(const RDSymbol* symbol, RendererItem* ritem) const;
        bool renderSymbolPointer(const RDSymbol* symbol, RendererItem* ritem) const;

    private:
        mutable std::string m_lastword, m_selectedtext, m_instructionstr, m_asmstr, m_rdilstr;
        mutable size_t m_commentcolumn{0}; // Make renderer update comment column dynamically
        Disassembler* m_disassembler;
        const Cursor* m_cursor;
        rd_flag m_flags;
};
