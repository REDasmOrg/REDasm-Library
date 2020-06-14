#pragma once

#include <array>
#include <rdcore/disassembler.h>
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
        bool renderItem(size_t index, RDRendererItem* ritem) const;
        bool selectedSymbol(RDSymbol* symbol) const;
        const std::string& getWordFromPosition(const RDCursorPos* pos, RDCursorRange* range) const;
        const std::string& getInstruction(rd_address address) const;
        const std::string& getSelectedText() const;
        const std::string& getCurrentWord() const;
        size_t getLastColumn(size_t index) const;
        rd_flag flags() const;

    private:
        void highlightSelection(RendererItem* ritem) const;
        void highlightCursor(RendererItem* ritem) const;
        void highlightWords(RendererItem* ritem) const;

    public:
        static void renderPrologue(RDRenderItemParams* rip);
        static void renderAddress(RDRenderItemParams* rip);
        static void renderAddressIndent(RDRenderItemParams* rip);
        static void renderIndent(RDRenderItemParams* rip, size_t n, bool ignoreflags = false);
        static bool renderRegister(RDRenderItemParams* rip, register_t r);
        static bool renderOperand(RDRenderItemParams* rip, const RDOperand* op);
        static bool renderConstant(RDRenderItemParams* rip, u64 c);
        static bool renderImmediate(RDRenderItemParams* rip, u64 imm);
        static bool renderMemory(RDRenderItemParams* rip, rd_address address);
        static bool renderMnemonic(RDRenderItemParams* rip);
        static bool renderDisplacement(RDRenderItemParams* rip);

    private:
        static bool renderSymbolPointer(RDRenderItemParams* rip);
        static void renderSymbolPrologue(RDRenderItemParams* rip);
        static bool renderSegment(RDRenderItemParams* rip);
        static bool renderFunction(RDRenderItemParams* rip);
        static bool renderInstruction(RDRenderItemParams* rip);
        static bool renderOperand(RDRenderItemParams* rip);
        static bool renderSymbol(RDRenderItemParams* rip);

    private:
        bool renderParams(RDRenderItemParams* rip) const;
        bool renderSeparator(const RDDocumentItem* item, RDRendererItem* ritem) const;
        bool renderUnexplored(const RDDocumentItem* item, RDRendererItem* ritem) const;
        void renderAddress(const RDDocumentItem* item, RendererItem* ritem) const;
        void renderAddressIndent(const RDDocumentItem* item, RendererItem* ritem) const;
        void renderIndent(RendererItem* ritem, size_t n, bool ignoreflags = false) const;
        static void renderComments(RDRenderItemParams* rip);
        static void renderSymbol(RDRenderItemParams* rip, rd_address address);

    private:
        static std::array<Renderer_Callback, DocumentItemType_Length> m_slots;
        mutable std::string m_lastword, m_selectedtext, m_instructionstr;
        mutable size_t m_commentcolumn{0}; // Make renderer update comment column dynamically
        Disassembler* m_disassembler;
        const Cursor* m_cursor;
        rd_flag m_flags;
};
