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
    public:
        Renderer(Disassembler* disassembler, const Cursor* cursor, flag_t flags);
        void render(size_t index, size_t count, Callback_Render cbrender, void* userdata) const;
        bool renderItem(size_t index, RDRendererItem* ritem) const;
        bool selectedSymbol(RDSymbol* symbol) const;
        const std::string& getWordFromPosition(const RDCursorPos* pos, RDCursorRange* range) const;
        const std::string& getInstruction(address_t address) const;
        const std::string& getSelectedText() const;
        const std::string& getCurrentWord() const;
        size_t getLastColumn(size_t index) const;
        flag_t flags() const;

    private:
        void highlightSelection(RendererItem* ritem) const;
        void highlightCursor(RendererItem* ritem) const;
        void highlightWords(RendererItem* ritem) const;

    public:
        static void renderAddress(RDRenderItemParams* rip);
        static void renderAddressIndent(RDRenderItemParams* rip);
        static void renderIndent(RDRenderItemParams* rip, size_t n);
        static bool renderConstant(RDRenderItemParams* rip);
        static bool renderImmediate(RDRenderItemParams* rip);
        static bool renderMemory(RDRenderItemParams* rip);
        static bool renderDisplacement(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);
        static bool renderRegister(const RDAssemblerPlugin*, RDRenderItemParams* rip);

    private:
        static void renderPrologue(RDRenderItemParams* rip);
        static bool renderMnemonic(const RDAssemblerPlugin*, RDRenderItemParams* rip);
        static bool renderSymbolPointer(RDRenderItemParams* rip);
        static void renderSymbolPrologue(RDRenderItemParams* rip);
        static bool renderSegment(const RDAssemblerPlugin*, RDRenderItemParams* rip);
        static bool renderFunction(const RDAssemblerPlugin*, RDRenderItemParams* rip);
        static bool renderInstruction(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);
        static bool renderOperand(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);
        static bool renderSymbol(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);

    private:
        template<typename ...Args> bool renderParams(type_t type, const RDDocumentItem* item, RDRendererItem* ritem, Args... args) const;
        bool renderParams(RDRenderItemParams* rip) const;
        bool renderSeparator(const RDDocumentItem* item, RDRendererItem* ritem) const;
        bool renderUnexplored(const RDDocumentItem* item, RDRendererItem* ritem) const;
        void renderAddress(const RDDocumentItem* item, RendererItem* ritem) const;
        void renderAddressIndent(const RDDocumentItem* item, RendererItem* ritem) const;
        void renderIndent(RendererItem* ritem, size_t n) const;
        static void renderComments(RDRenderItemParams* rip);
        static void renderSymbol(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip, address_t address);
        static void renderRegister(RDRenderItemParams* rip, register_t r);

    private:
        static std::array<Callback_RenderItem, DocumentItemType_Length> m_slots;
        mutable std::string m_lastword, m_selectedtext, m_instructionstr;
        Disassembler* m_disassembler;
        const Cursor* m_cursor;
        flag_t m_flags;
};

template<typename ...Args>
bool Renderer::renderParams(type_t type, const RDDocumentItem* item, RDRendererItem* ritem, Args... args) const
{
    if(type >= m_slots.size()) return false;

    RDRenderItemParams rip = { type, CPTR(const RDRenderer, this), CPTR(RDDisassembler, m_disassembler), item, ritem, args... };
    return this->renderParams(&rip);
}
