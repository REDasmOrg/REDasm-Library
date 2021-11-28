#pragma once

#include <algorithm>
#include <string>
#include <deque>
#include <rdapi/renderer/renderer.h>
#include <rdapi/config.h>
#include "../document/document_fwd.h"
#include "../object.h"
#include "common.h"

#define HEXDUMP_LENGTH 0x10

struct SurfaceColumns;
class Assembler;
class SurfaceRenderer;

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
        Renderer(Context* ctx, SurfaceRow& sfrow, rd_flag flags);
        Renderer(SurfaceRenderer* surface, SurfaceRow& sfrow, rd_flag flags);
        ~Renderer();

    public: // C-API Methods
        void renderAssemblerInstruction();
        void renderRDILInstruction();
        void renderSigned(s64 value);
        void renderUnsigned(u64 value);
        void renderReference(rd_location loc);
        void renderMnemonic(const std::string& s, rd_type theme);
        void renderMnemonicWord(const std::string& s, rd_type theme);
        void renderRegister(const std::string& s);
        void renderConstant(const std::string& s);
        void renderText(const std::string& s, rd_type theme = Theme_Default);
        void renderIndent(size_t n, bool ignoreflags = false);
        void renderUnknown();

    public: // SurfaceRenderer Methods
        void renderSegment();
        void renderSeparator();
        void renderInstruction();
        void renderLocation();
        void renderFunction();
        void renderString();
        void renderData();
        void renderUnknown(size_t size);
        void renderLine(const std::string& s);
        void renderTypeField();
        void renderType();

    private:
        inline rd_address address() const { return m_sfrow.address; }
        std::string renderLabel(u8 theme = Theme_Label);
        bool hasFlag(rd_flag f) const;
        bool renderInstrIndent(const std::string& diffstr, bool ignoreflags = false);
        void compileParams(RDRendererParams* srp);
        void renderValue(rd_address address, size_t size);
        void renderLabelIndent();
        void renderPrologue();
        void renderComments();

    public:
        static std::string getInstruction(Context* ctx, rd_address address);
        static std::string getAssemblerInstruction(Context* ctx, rd_address address);
        static std::string getRDILInstruction(Context* ctx, rd_address address);

    private:
        Renderer& chunk(const std::string& s, u8 fg = Theme_Default, u8 bg = Theme_Default);
        Renderer& chunkalign(const std::string& s, u8 fg = Theme_Default, u8 bg = Theme_Default);
        SafeDocument& document() const;

    private:
        mutable std::string m_asminstruction, m_rdilinstruction;
        size_t m_segmentidx;
        SurfaceRenderer* m_surface;
        SurfaceRow& m_sfrow;
        std::vector<std::string> m_autocomments;
        u8 m_currentfg{Theme_Default}, m_currentbg{Theme_Default};
        rd_flag m_flags;
};
