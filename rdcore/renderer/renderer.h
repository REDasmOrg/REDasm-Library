#pragma once

#include <algorithm>
#include <string>
#include <deque>
#include <rdapi/renderer/renderer.h>
#include <rdapi/config.h>
#include "../document/document_fwd.h"
#include "../object.h"

class Disassembler;
class Assembler;

class Renderer: public Object
{
    public:
        struct SurfaceChunk { u8 background; u8 foreground; std::string chunk; };
        typedef std::deque<SurfaceChunk> Chunks;

    private:
        struct StyleScope {
            StyleScope(Renderer* r, u8 fg = Theme_Default, u8 bg = Theme_Default): m_renderer(r) {
                m_prevfg = m_renderer->m_currentfg; std::swap(fg, m_renderer->m_currentfg);
                m_prevbg = m_renderer->m_currentbg; std::swap(bg, m_renderer->m_currentbg);
            }

            ~StyleScope() {
                std::swap(m_renderer->m_currentfg, m_prevfg);
                std::swap(m_renderer->m_currentbg, m_prevbg);
            }

            private:
                Renderer* m_renderer;
                u8 m_prevfg, m_prevbg;
        };

    public:
        Renderer(Context* ctx, rd_flag flags);
        bool render(const RDDocumentItem* item);
        const std::string& text() const;
        const Chunks& chunks() const;

    public:
        void renderHexDump(const RDBufferView* view, size_t size);
        void renderAssemblerInstruction(rd_address address);
        void renderRDILInstruction(rd_address address);
        void renderSigned(s64 value);
        void renderUnsigned(u64 value);
        void renderReference(rd_location loc);
        void renderMnemonic(const std::string& s, rd_type theme);
        void renderMnemonicWord(const std::string& s, rd_type theme);
        void renderRegister(const std::string& s);
        void renderConstant(const std::string& s);
        void renderText(const std::string& s, rd_type theme = Theme_Default);
        void renderUnknown();
        void renderIndent(size_t n, bool ignoreflags = false);

    private:
        void renderSegment(const RDDocumentItem* item);
        void renderFunction(const RDDocumentItem* item);
        void renderInstruction(const RDDocumentItem* item);
        void renderSymbol(const RDDocumentItem* item);
        void renderUnknown(const RDDocumentItem* item);
        void renderSeparator(const RDDocumentItem* item);
        void renderType(const RDDocumentItem* item);

    private:
        void renderPrologue(rd_address address);
        void renderBlock(rd_address address);
        void renderAddressIndent(rd_address address);
        void renderComments(rd_address address);
        void renderSymbolValue(const RDSymbol* symbol);
        bool renderSymbolPointer(const RDSymbol* symbol);
        void compileParams(rd_address address, RDRendererParams* srp);
        bool hasFlag(rd_flag f) const;

    private:
        Disassembler* disassembler() const;
        Assembler* assembler() const;
        SafeDocument& document() const;
        Renderer& chunk(const std::string& s, u8 fg = Theme_Default, u8 bg = Theme_Default);

    public:
        static std::string getInstruction(Context* ctx, rd_address address);
        static std::string getAssemblerInstruction(Context* ctx, rd_address address);
        static std::string getRDILInstruction(Context* ctx, rd_address address);

    private:
        mutable std::string m_asminstruction, m_rdilinstruction;
        u8 m_currentfg{Theme_Default}, m_currentbg{Theme_Default};
        std::deque<SurfaceChunk> m_tokens;
        std::string m_text;
        rd_flag m_flags;
};

