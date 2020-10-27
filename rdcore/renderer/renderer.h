#pragma once

#include <algorithm>
#include <string>
#include <deque>
#include <rdapi/renderer/surface.h>
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
        Renderer(Context* ctx, rd_flag flags, int* commentcolumn);
        bool render(const RDDocumentItem* item);
        const std::string& text() const;
        const Chunks& chunks() const;

    public:
        void renderHexDump(const RDBufferView* view, size_t size);
        void renderAssemblerInstruction(rd_address address);
        void renderRDILInstruction(rd_address address);
        void renderSigned(s64 value);
        void renderUnsigned(u64 value);
        void renderMnemonic(const std::string& s, rd_type theme);
        void renderRegister(const std::string& s);
        void renderConstant(const std::string& s);
        void renderText(const std::string& s, rd_type theme = Theme_Default);
        void renderIndent(size_t n, bool ignoreflags = false);

    private:
        void renderSegment(rd_address address);
        void renderFunction(rd_address address);
        void renderInstruction(rd_address address);
        void renderSymbol(rd_address address);
        void renderUnexplored(rd_address address);
        void renderSeparator(rd_address address);

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

    private:
        u8 m_currentfg{Theme_Default}, m_currentbg{Theme_Default};
        int* m_commentcolumn;
        std::deque<SurfaceChunk> m_tokens;
        std::string m_text;
        rd_flag m_flags;
};

