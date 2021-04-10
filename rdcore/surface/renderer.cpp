#include "renderer.h"
#include "surfacerenderer.h"
#include "../document/document.h"
#include "../plugin/assembler.h"
#include "../rdil/ilfunction.h"
#include "../rdil/rdil.h"
#include "../support/utils.h"
#include "../context.h"
#include <rdapi/renderer/surface.h>
#include <cstring>
#include <cctype>

#define INDENT_COMMENT     10
#define STRING_THRESHOLD   48
#define SEPARATOR_LENGTH   50
#define UNKNOWN_STRING     "???"

Renderer::Renderer(Context* ctx, SurfaceRow& sfrow, rd_flag flags): Object(ctx), m_surface(nullptr), m_sfrow(sfrow), m_flags(flags) { this->renderPrologue(); }
Renderer::Renderer(SurfaceRenderer* surface, SurfaceRow& sfrow, rd_flag flags): Object(surface->context()), m_surface(surface), m_sfrow(sfrow), m_flags(flags) { this->renderPrologue(); }

Renderer::~Renderer()
{
    int col = 0;

    for(size_t i = 0; i < m_sfrow.chunks.size(); i++)
    {
        const auto& c = m_sfrow.chunks[i];

        for(const auto& ch : c.chunk)
        {
            if(!m_surface || (col >= m_surface->firstColumn()))
                m_sfrow.cells.push_back({ c.background, c.foreground, ch });

            col++;
        }
    }

    if(m_surface) m_surface->setLastColumn(m_sfrow.cells.size());
}

void Renderer::renderAssemblerInstruction()
{
    RDRendererParams srp;
    this->compileParams(&srp);

    if(!this->assembler()->renderInstruction(&srp))
        this->chunk(UNKNOWN_STRING);
}

void Renderer::renderRDILInstruction()
{
    RDRendererParams srp;
    this->compileParams(&srp);

    ILFunction il(this->context());
    this->context()->assembler()->lift(srp.address, &srp.view, &il);

    for(size_t i = 0; i < il.size(); i++)
    {
        if(i) this->chunk("; "); // Attach more statements, if needed
        RDIL::render(il.expression(i), this, this->address());
    }
}

void Renderer::renderSigned(s64 value) { this->chunk(Utils::hex(value), Theme_Constant); }
void Renderer::renderUnsigned(u64 value) { this->chunk(Utils::hex(value), Theme_Constant); }

void Renderer::renderReference(rd_location loc)
{
    auto label = this->document()->getLabel(loc);

    if(label)
    {
        this->chunk(*label, Theme_Label);

        auto flags = this->document()->getFlags(loc);
        if(flags & AddressFlags_AsciiString) m_autocomments.push_back(Utils::quoted(this->document()->readString(loc, STRING_THRESHOLD)));
        if(flags & AddressFlags_WideString) m_autocomments.push_back(Utils::quoted(this->document()->readWString(loc, STRING_THRESHOLD)));
    }
    else this->chunk(Utils::hex(loc), Theme_Constant);
}

void Renderer::renderMnemonic(const std::string& s, rd_type theme)
{
    if(s.empty()) return;

    this->chunk(s, theme);

    auto& ss = this->context()->surfaceState();
    ss.mnemonicendcol = std::max(ss.mnemonicendcol, s.size());

    size_t diff = ss.mnemonicendcol - s.size();
    if(diff) this->chunk(std::string(diff, ' '));
}

void Renderer::renderMnemonicWord(const std::string& s, rd_type theme)
{
    this->renderMnemonic(s, theme);
    this->chunk(" ");
}

void Renderer::renderRegister(const std::string& s) { this->chunk(s, Theme_Reg); }
void Renderer::renderConstant(const std::string& s) { this->chunk(s, Theme_Constant); }
void Renderer::renderText(const std::string& s, rd_type theme) { this->chunk(s, theme); }
void Renderer::renderUnknown() { this->chunk(UNKNOWN_STRING); }

void Renderer::renderSegment()
{
    StyleScope s(this, Theme_Segment);
    RDSegment segment;

    if(this->document()->addressToSegment(this->address(), &segment))
    {
        this->chunkalign(segment.name);

        this->chunk("segment").chunk(" ")
             .chunk("(")
             .chunk("START").chunk(":").chunk(" ").chunk(Utils::hex(segment.address, this->assembler()->bits())).chunk(", ")
             .chunk("END").chunk(":").chunk(" ").chunk(Utils::hex(segment.endaddress, this->assembler()->bits()))
             .chunk(")");
    }
    else
    {
        std::string name = Utils::hex(this->address());
        this->chunkalign(name);
        this->chunk(" ").chunk("segment");
    }
}

void Renderer::renderSeparator() { this->chunk(std::string(SEPARATOR_LENGTH, '-'), Theme_Nop); }

void Renderer::renderInstruction()
{
    this->renderInstrIndent(std::string());

    if(this->context()->flags() & ContextFlags_ShowRDIL)
        this->renderRDILInstruction();
    else
        this->renderAssemblerInstruction();

    this->renderComments();
}

void Renderer::renderLocation() { this->renderLabel(); this->chunk(":"); }

void Renderer::renderFunction()
{
    std::string name = this->renderLabel(Theme_Function);
    StyleScope s(this, Theme_Function);
    this->chunk(" ");
    this->renderInstrIndent(name + " ");
    this->chunk("function");
}

void Renderer::renderString()
{
    this->renderLabel();
    this->chunk(" ");

    RDBlock block;

    if(this->document()->addressToBlock(this->address(), &block))
    {
        rd_flag flags = this->document()->getFlags(this->address());
        size_t len = BlockContainer::size(&block);

        std::string s;
        if(flags == AddressFlags_WideString) s = Utils::simplified(this->document()->readWString(this->address(), len));
        else s = Utils::simplified(this->document()->readString(this->address(), len));
        this->chunk(Utils::quoted(s), Theme_String);
    }
    else
        this->chunk(UNKNOWN_STRING);
}

void Renderer::renderData()
{
    this->renderLabel();
    this->chunk(" ");

    RDBufferView view;

    if(this->document()->getBlockView(this->address(), &view))
        this->renderValue(this->address(), view.size);
    else
        this->renderUnknown();
}

void Renderer::renderLine(const std::string& s)
{
    this->renderInstrIndent(std::string());
    this->renderText(s);
}

void Renderer::renderTypeField()
{
    int indent = 0;
    auto* typefield = this->document()->getTypeField(this->address(), &indent);

    this->renderIndent(indent * 2, true);
    if(typefield) this->chunk(typefield->name(), Theme_Label).chunk(" ").chunk(typefield->typeName(), Theme_Type);
    else this->chunk("field_" + Utils::hex(this->address()));

    if(!typefield) return;
    this->chunk(" ");

    switch(typefield->type())
    {
        case Type_AsciiString: {
            std::string s = Utils::simplified(this->document()->readString(this->address(), typefield->size()));
            this->chunk(Utils::quoted(s), Theme_String);
            break;
        }

        case Type_WideString: {
            std::string s = Utils::simplified(this->document()->readWString(this->address(), typefield->size()));
            this->chunk(Utils::quoted(s), Theme_String);
            break;
        }

        case Type_Int:
            this->renderValue(this->address(), typefield->size());
            break;

        default:
            this->chunk("Type#" + typefield->typeName());
            break;
    }
}

void Renderer::renderUnknown(size_t size)
{
    RDBlock block;

    if(!this->document()->addressToBlock(this->address(), &block))
    {
        this->renderUnknown();
        return;
    }

    switch(size)
    {
        case 1: this->chunk("byte",  Theme_Nop); break;
        case 2: this->chunk("word",  Theme_Nop); break;
        case 4: this->chunk("dword", Theme_Nop); break;
        case 8: this->chunk("qword", Theme_Nop); break;

        default:
            this->chunk("bytes", Theme_Nop).chunk(" ");
            this->chunk("(").chunk(Utils::hex(size), Theme_Constant).chunk(")");
            return;
    }

    this->chunk(" ");

    RDBufferView view;

    if(this->document()->getBlockView(block.address, &view))
        this->chunk(Utils::hexStringEndian(this->context(), &view), Theme_Constant);
    else
        this->renderUnknown();
}

void Renderer::renderType()
{
    auto* type = this->document()->getType(this->address());

    if(!type)
    {
        this->chunk("Type not Found");
        return;
    }

    this->chunk(type->name(), Theme_Label)
         .chunk(" ")
         .chunk(type->typeName(), Theme_Type);
}

std::string Renderer::renderLabel(u8 theme)
{
    auto label = this->document()->getLabel(this->address());
    std::string name = label ? *label : Document::makeLabel(this->address(), "unk");
    this->chunk(name, theme);
    return name;
}

void Renderer::renderLabelIndent()
{
    auto name = this->renderLabel();

    if(!this->renderInstrIndent(name, true))
        this->chunk(" ");
}

bool Renderer::renderInstrIndent(const std::string& diffstr, bool ignoreflags)
{
    auto& ss = this->context()->surfaceState();
    if(ss.instrstartcol <= diffstr.size()) return false;
    this->renderIndent(ss.instrstartcol - diffstr.size(), ignoreflags);
    return true;
}

void Renderer::renderIndent(size_t n, bool ignoreflags)
{
    if(!ignoreflags && this->hasFlag(RendererFlags_NoIndent)) return;
    this->chunk(std::string(n, ' '));
}

void Renderer::renderPrologue()
{
    if(!this->hasFlag(RendererFlags_NoSegmentColumn))
    {
        RDSegment s;
        if(this->document()->addressToSegment(this->address(), &s)) this->chunk(s.name);
        else this->renderUnknown();
        this->chunk(":");
    }

    if(!this->hasFlag(RendererFlags_NoAddressColumn))
        this->chunk(Utils::hex(this->address(), this->assembler()->bits()));

    if(!this->hasFlag(RendererFlags_NoSegmentColumn) || !this->hasFlag(RendererFlags_NoAddressColumn))
        this->chunk(" ");
}

void Renderer::renderComments()
{
    if(this->hasFlag(RendererFlags_NoComments)) return;

    std::string autocomments = Utils::join(m_autocomments, COMMENT_SEPARATOR);
    std::string usercomments = Utils::join(Utils::split(this->document()->getComments(this->address()), '\n'), COMMENT_SEPARATOR);
    if(autocomments.empty() && usercomments.empty()) return;

    this->chunk(std::string(INDENT_COMMENT, ' '));
    if(!autocomments.empty()) this->chunk(Utils::simplified(autocomments), Theme_AutoComment);

    if(!usercomments.empty())
    {
        if(!autocomments.empty()) this->chunk(COMMENT_SEPARATOR);
        this->chunk("# ", Theme_Comment).chunk(Utils::simplified(usercomments), Theme_Comment);
    }
}

void Renderer::compileParams(RDRendererParams* srp)
{
    *srp = { this->address(),
             { },
             CPTR(const RDContext, this->context()),
             CPTR(RDRenderer, this) };

    this->context()->document()->getView(this->address(), RD_NVAL, &srp->view);
}

void Renderer::renderValue(rd_address address, size_t size)
{
    RDBufferView view;

    if(!this->document()->getView(address, size, &view))
    {
        this->renderUnknown();
        return;
    }

    auto flags = this->document()->getFlags(this->address());

    if((flags & AddressFlags_Pointer) || (size == this->context()->addressWidth()))
    {
        std::optional<std::string> name;
        auto loc = this->document()->dereference(address);

        if(loc.valid && (name = this->document()->getLabel(loc.address)); name)
        {
            this->chunk(*name, Theme_Label);
            return;
        }
    }

    switch(view.size)
    {
        case 1:
        case 2:
        case 4:
        case 8:
            this->chunk(Utils::hexStringEndian(this->context(), &view), Theme_Constant);
            break;

        default:
            this->renderUnknown();
            break;
    }
}

bool Renderer::hasFlag(rd_flag f) const { return m_flags & f; }
SafeDocument& Renderer::document() const { return this->context()->document(); }
Assembler* Renderer::assembler() const { return this->context()->assembler(); }

Renderer& Renderer::chunk(const std::string& s, u8 fg, u8 bg)
{
    if(fg == Theme_Default) fg = m_currentfg;
    if(bg == Theme_Default) bg = m_currentbg;

    m_sfrow.text += s;
    m_sfrow.chunks.push_back({ bg, fg, s });
    return *this;
}

Renderer& Renderer::chunkalign(const std::string& s, u8 fg, u8 bg)
{
    this->chunk(s, fg, bg);
    if(!this->renderInstrIndent(s)) this->chunk(" ");
    return *this;
}

std::string Renderer::getInstruction(Context* ctx, rd_address address)
{
    if(ctx->flags() & ContextFlags_ShowRDIL) return Renderer::getRDILInstruction(ctx, address);
    return Renderer::getAssemblerInstruction(ctx, address);
}

std::string Renderer::getAssemblerInstruction(Context* ctx, rd_address address)
{
    SurfaceRow row(address);
    Renderer r(ctx, row, RendererFlags_Simplified);
    r.renderAssemblerInstruction();
    return row.text;
}

std::string Renderer::getRDILInstruction(Context* ctx, rd_address address)
{
    SurfaceRow row(address);
    Renderer r(ctx, row, RendererFlags_Simplified);
    r.renderRDILInstruction();
    return row.text;
}
